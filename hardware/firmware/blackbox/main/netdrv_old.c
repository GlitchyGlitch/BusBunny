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

// TODO: prevent reset on return

static void handle_recv(void *task_param)
{
  net_task_param_t *param;
  param = (net_task_param_t *)task_param;
  ssize_t rx_len = 0;
  char rx_buffer[128];

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
      rx_buffer[rx_len] = 0;
      xQueueSend(param->queue, (void *)&rx_buffer, 10);
    }
  } while (rx_len > 0);

  shutdown(param->sockfd, 0);
  close(param->sockfd);

  esp_free(param);
  vTaskDelete(NULL);
}

static void handle_send(void *task_param)
{
  net_task_param_t *param;
  param = (net_task_param_t *)task_param;
  char tx_buffer[200]; // TODO: chceck size
  bool error = false;  // TODO: Figure out how to make it cleaner

  while (!error && param->sockfd)
  {
    BaseType_t ok = xQueueReceive(param->queue, &tx_buffer, (TickType_t)10);
    size_t to_write, len;
    to_write = len = strlen(tx_buffer);
    while (!error && to_write > 0 && ok == pdTRUE) // TODO: Figure out how to make it cleaner
    {
      ssize_t written = send(param->sockfd, tx_buffer + (len - to_write), to_write, 0);
      if (written < 0)
      {
        ESP_LOGW(TAG, "Connection closed"); // TODO: check errno to find out
        error = true;
      }
      to_write -= written;
    }
  }

  shutdown(param->sockfd, 0);
  close(param->sockfd);

  esp_free(param);
  vTaskDelete(NULL);
}

netdrv_err_t
netdrv_create(netdrv_t *net, ipstr_t ip, uint16_t port, size_t rx_buffer_size)
{
  net->ip = ip;
  net->rx_buffer_size = rx_buffer_size;
  // inet_ntoa_r(net->dst_addr.sin_addr, net->ip.bytes, sizeof(ip) - 1);
  net->dst_addr.sin_addr.s_addr = htonl(INADDR_ANY);
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

  QueueHandle_t queue_recv = xQueueCreate(30, 128);
  QueueHandle_t queue_send = xQueueCreate(30, 128);

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