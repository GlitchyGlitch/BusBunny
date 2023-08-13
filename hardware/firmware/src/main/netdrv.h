#pragma once
#include "netdrv_err.h"
#include "esp_system.h"
#include "lwip/sockets.h"

#define MESSAGE_SIZE 255
#define RAW_MESSAGE_SIZE 256
typedef SemaphoreHandle_t netdrv_err_semaphore_handle_t;

typedef char netdrv_raw_msg_t[RAW_MESSAGE_SIZE];
typedef int32_t netdrv_sockfd_t;
typedef int32_t netdrv_err_t;
typedef uint16_t netdrv_port_t;

typedef struct netdrv_task_param
{
  QueueHandle_t queue;
  netdrv_err_semaphore_handle_t err_semaphore;
  netdrv_sockfd_t sockfd;
} netdrv_task_param_t;

typedef struct netdrv_ipstr
{
  char bytes[32];
} netdrv_ipstr_t; // TODO: review if this type makes any sense same with port

typedef struct netdrv
{
  size_t rx_buffer_size;
  netdrv_ipstr_t ip;
  netdrv_sockfd_t sockfd;
  struct sockaddr_in dst_addr;
} netdrv_t;

typedef struct netdrv_queue
{
  QueueHandle_t queue_recv;
  QueueHandle_t queue_send;
  netdrv_err_t err;
} netdrv_queue_t;

typedef struct netdrv_msg
{
  size_t size;
  char data[MESSAGE_SIZE];
} netdrv_msg_t;

netdrv_err_t netdrv_create(netdrv_t *net, netdrv_ipstr_t ip, netdrv_port_t port, size_t rx_buffer_size);
netdrv_err_t netdrv_listen(netdrv_t *net);
netdrv_queue_t netdrv_accept(netdrv_t *net);
