#include <stdint.h>

#include "netdrv.h"
#include "system.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"

#include "lwip/sockets.h"

static const char *TAG = "netdrv";

// PRIVATE
/// @brief Turn struct into byte array ready to send over network
/// @param msg Message struct
/// @return Remember to free buffer after using function!
static netdrv_raw_msg_t *netdrv_serialize_msg(netdrv_msg_t msg)
{
  netdrv_raw_msg_t *raw_msg = malloc(sizeof(netdrv_raw_msg_t));
  (*raw_msg)[0] = (char)msg.size;
  memcpy(&(*raw_msg)[1], msg.data, msg.size);
  return raw_msg;
}

/// @brief Turn byte array into struct ready to process by system
/// @param msg Byte buffer
/// @return Remember to free after using function!
static netdrv_msg_t netdrv_deserialize_msg(netdrv_raw_msg_t *raw_msg)
{
  netdrv_msg_t msg;
  msg.size = (int)(*raw_msg)[0];
  memcpy(msg.data, &(*raw_msg)[1], msg.size);
  return msg;
}

static ssize_t netdrv_send(netdrv_sockfd_t sockfd, netdrv_msg_t msg) {
  netdrv_raw_msg_t *ready_msg = netdrv_serialize_msg(msg);
  ssize_t bytes_sent = send(sockfd, ready_msg, msg.size+1, 0);
  esp_free(ready_msg);
  return bytes_sent;
}

// TASKS
static void handle_recv(void *task_param)
{
  netdrv_task_param_t *param = (netdrv_task_param_t *)task_param;
  netdrv_raw_msg_t raw_msg;
  ssize_t rx_len = 0;

  for (;;)
  {
    rx_len = recv(param->sockfd, raw_msg, sizeof(raw_msg), 0);

    if (rx_len < 0)
    {
      ESP_LOGE(TAG, "Error occurred during receiving: errno %d", errno);
      xSemaphoreGive(param->err_semaphore);
      break;
    }
    
    if (rx_len == 0)
    {
      ESP_LOGW(TAG, "Connection closed %d", rx_len);
      xSemaphoreGive(param->err_semaphore);
      break;
    }

    netdrv_msg_t msg = netdrv_deserialize_msg(&raw_msg);
    xQueueSend(param->queue, &msg, 100);
  }
  
  xSemaphoreGive(param->err_semaphore);
  shutdown(param->sockfd, 0);
  close(param->sockfd);

  esp_free(param);
  vTaskDelete(NULL);
}

static void handle_send(void *task_param)
{
  netdrv_task_param_t *param = (netdrv_task_param_t *)task_param;

  netdrv_msg_t msg_buffer;

  for (;;)
  {
    BaseType_t ok = xQueueReceive(param->queue, &msg_buffer, 100);
    
    if (ok == pdFALSE)
    {
      continue;
    }

    if (netdrv_send(param->sockfd, msg_buffer) < 0)
    {
      ESP_LOGE(TAG, "Error during send");
      break;
    }
  }
  xSemaphoreGive(param->err_semaphore);
  esp_free(param);
  vTaskSuspend(NULL);
}

static void handle_sv(void *task_param)
{
  netdrv_task_param_t *param = (netdrv_task_param_t *)task_param;

  for(;;)
  {
    if (xSemaphoreTake(param->err_semaphore, pdMS_TO_TICKS(100)) == pdTRUE)
    {
      vTaskSuspend("netdrv_recv");
      vTaskSuspend("netdrv_send ");
      vTaskSuspend("netdrv_sv");
      ESP_LOGE("DEBUG", "noszku");
    }
  }
}

// PUBLIC
netdrv_err_t netdrv_create(netdrv_t *net, netdrv_ipstr_t ip, netdrv_port_t port, size_t rx_buffer_size)
{
  // TODO: reconfigure DHCP

  net->ip = ip;
  net->rx_buffer_size = rx_buffer_size;
  net->dst_addr.sin_addr.s_addr = inet_addr(ip.bytes);
  net->dst_addr.sin_family = AF_INET;   // Define address family as Ipv4
  net->dst_addr.sin_port = htons(port); // Define PORT

  net->sockfd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (net->sockfd < 0)
  {
    ESP_LOGE(TAG, "Unable to create socket: errno %d", errno);
    return NETDRV_ERR_CREATE;
  }
  ESP_LOGI(TAG, "Socket created");

  int32_t opt = 1;
  setsockopt(net->sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
  ESP_LOGI(TAG, "Socket configured");

  int32_t bind_err = bind(net->sockfd, (struct sockaddr *)&net->dst_addr, sizeof(net->dst_addr));
  if (bind_err != 0)
  {
    ESP_LOGE(TAG, "Socket unable to bind: errno %d", errno);
    return NETDRV_ERR_BIND;
  }
  ESP_LOGI(TAG, "Socket bound");
  ESP_LOGI(TAG, "IPv4: %s", ip.bytes);
  ESP_LOGI(TAG, "Port: %d", port);
  return NETDRV_OK;
}

netdrv_err_t netdrv_listen(netdrv_t *net)
{
  int32_t listen_err = listen(net->sockfd, 1);
  if (listen_err != 0)
  {
    ESP_LOGE(TAG, "Error occured during listen: errno %d", errno);
  }
  ESP_LOGI(TAG, "Socket listening");

  return NETDRV_OK;
}

netdrv_queue_t netdrv_accept(netdrv_t *net)
{
  netdrv_queue_t ret = (netdrv_queue_t){NULL, NULL, NETDRV_ERR_ACCEPT};
  struct sockaddr_in client_addr;
  size_t client_addr_len = sizeof(client_addr);
  netdrv_err_semaphore_handle_t err_semaphore = malloc(sizeof(err_semaphore)); // TODO: Remember to dealocate
  err_semaphore = xSemaphoreCreateBinary();
  netdrv_sockfd_t client_sockfd = accept(net->sockfd, (struct sockaddr *)&client_addr, &client_addr_len);

  if (client_sockfd < 0)
  {
    ESP_LOGE(TAG, "Unable to accept connection: errno %d", errno);
    return ret;
  }

  QueueHandle_t queue_recv = xQueueCreate(30, sizeof(netdrv_msg_t));
  QueueHandle_t queue_send = xQueueCreate(30, sizeof(netdrv_msg_t));

  netdrv_task_param_t *task_param_recv = (netdrv_task_param_t *)malloc(sizeof(netdrv_task_param_t));
  netdrv_task_param_t *task_param_send = (netdrv_task_param_t *)malloc(sizeof(netdrv_task_param_t));
  netdrv_task_param_t *task_param_sv = (netdrv_task_param_t *)malloc(sizeof(netdrv_task_param_t));


  task_param_recv->queue = queue_recv;
  task_param_recv->sockfd = client_sockfd;
  task_param_recv->err_semaphore = err_semaphore;
  task_param_send->queue = queue_send;
  task_param_send->sockfd = client_sockfd;
  task_param_send->err_semaphore = err_semaphore;
  task_param_sv->sockfd = client_sockfd;
  task_param_sv->err_semaphore = err_semaphore;


  xTaskCreate(handle_recv, "netdrv_recv", 61440, (void *)task_param_recv, 200, NULL);
  xTaskCreate(handle_send, "netdrv_send", 4096, (void *)task_param_send, 200, NULL);
  xTaskCreate(handle_sv, "netdrv_sv", 2048, (void *)task_param_sv, 1, NULL);


  ret.queue_recv = queue_recv; // TODO: Check tasks and queues before returning them
  ret.queue_send = queue_send;
  ret.err = NETDRV_OK;

  return ret;
}
