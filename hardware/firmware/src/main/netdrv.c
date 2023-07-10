#include <stdint.h>

#include "netdrv.h"
#include "system.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#include "esp_log.h"

#include "lwip/sockets.h"

static const char *TAG = "netdrv";

typedef struct net_task_param
{
  QueueHandle_t queue;
  sockfd_t sockfd;
} net_task_param_t;

/// @brief Turn struct into byte array ready to send over network
/// @param msg Message struct
/// @return Remember to free buffer after using method!
net_raw_msg_t *netdrv_serialize_msg(net_msg_t msg)
{
  net_raw_msg_t *raw_msg = malloc(sizeof(net_raw_msg_t));
  (*raw_msg)[0] = (char)msg.size;
  memcpy(&(*raw_msg)[1], msg.data, msg.size);
  return raw_msg;
}

// TODO: prevent reset on return

static void handle_recv(void *task_param)
{
  net_task_param_t *param;
  param = (net_task_param_t *)task_param;
  ssize_t rx_len = 0;
  char rx_buffer[128]; // TODO: Export this as kconfig

  do
  {
    if (param->sockfd)
    {
      rx_len = recv(param->sockfd, rx_buffer, sizeof(rx_buffer) - 1, 0);
    }

    if (rx_len < 0)
    {
      ESP_LOGE(TAG, "Error occurred during receiving: errno %d", errno);
    }
    else if (rx_len == 0)
    {
      ESP_LOGW(TAG, "Connection closed");
    }
    else
    { // TODO: bzero buffer maybe
      rx_buffer[rx_len] = 0; // TODO: Why? For string? Not necessary?
      xQueueSend(param->queue, (void *)&rx_buffer, 10);
    }
  } while (rx_len > 0);

  shutdown(param->sockfd, 0);
  close(param->sockfd);

  esp_free(param);
  vTaskDelete(NULL);
}

static void handle_send(void *task_param) // TODO: On heavy load, sometimes two messages are interpreted as one
{
  net_task_param_t *param;
  param = (net_task_param_t *)task_param;

  bool error = false;
  net_msg_t msg_buffer;

  while (!error && param->sockfd > 0)
  {
    BaseType_t ok = xQueueReceive(param->queue, &msg_buffer, (TickType_t)10); // TODO: send struct over queue
    net_raw_msg_t *ready_msg = netdrv_serialize_msg(msg_buffer);
    uint16_t ready_msg_len = msg_buffer.size + 1;
    ESP_LOGI("Tag", "Zawartość bufora: %s", msg_buffer.data);
    // TODO: Rethink this shit!
    while (!error && ready_msg_len > 0 && ok == pdTRUE) // FIXME: Figure out how to make it cleaner -> comparison of pointer with zero
    {
      ssize_t sent = send(param->sockfd, ready_msg, ready_msg_len, 0); // FIXME his panicing here
      if (sent < 0)
      {

        ESP_LOGE(TAG, "Error during send"); // TODO: check errno to find out
        error = true;
        esp_free(ready_msg);
      }
      ready_msg_len -= sent;
    }
    esp_free(ready_msg);
  }

  shutdown(param->sockfd, 0);
  close(param->sockfd);

  esp_free(param);
  vTaskDelete(NULL);
}

netdrv_err_t
netdrv_create(netdrv_t *net, ipstr_t ip, port_t port, size_t rx_buffer_size)
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

net_queue_t netdrv_accept(netdrv_t *net)
{
  // TODO track down exact size of packet and adjust values of reveive length and stack size
  net_queue_t ret = (net_queue_t){NULL, NULL, NETDRV_ERR_ACCEPT};
  struct sockaddr_in client_addr;
  size_t client_addr_len = sizeof(client_addr);
  sockfd_t client_sockfd = accept(net->sockfd, (struct sockaddr *)&client_addr, &client_addr_len);

  if (client_sockfd < 0)
  {
    ESP_LOGE(TAG, "Unable to accept connection: errno %d", errno);
    return ret;
  }

  QueueHandle_t queue_recv = xQueueCreate(30, sizeof(net_msg_t));
  QueueHandle_t queue_send = xQueueCreate(30, sizeof(net_msg_t));

  net_task_param_t *task_param_recv = (net_task_param_t *)malloc(sizeof(net_task_param_t));
  net_task_param_t *task_param_send = (net_task_param_t *)malloc(sizeof(net_task_param_t));

  task_param_recv->queue = queue_recv;
  task_param_recv->sockfd = client_sockfd;
  task_param_send->queue = queue_send;
  task_param_send->sockfd = client_sockfd;

  xTaskCreate(handle_recv, "netdrv_recv", 61440, (void *)task_param_recv, 1, NULL);
  xTaskCreate(handle_send, "netdrv_send", 4096, (void *)task_param_send, 1, NULL);

  ret.queue_recv = queue_recv; // TODO: Check tasks and queues before returning them
  ret.queue_send = queue_send;
  ret.err = NETDRV_OK;

  return ret;
}