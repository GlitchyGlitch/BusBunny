#pragma once
#include "netdrv_err.h"
#include "esp_system.h"
#include "lwip/sockets.h"

typedef int32_t sockfd_t;
typedef int32_t netdrv_err_t;

typedef struct ipstr
{
  char bytes[32];
} ipstr_t;

typedef struct netdrv
{
  size_t rx_buffer_size;
  ipstr_t ip; // char array to store client IP
  sockfd_t sockfd;
  struct sockaddr_in dst_addr;
} netdrv_t;

typedef struct net_queue
{
  QueueHandle_t queue_recv;
  QueueHandle_t queue_send;
  netdrv_err_t err;
} net_queue_t;

netdrv_err_t netdrv_create(netdrv_t *net, ipstr_t ip, uint16_t port, size_t rx_buffer_size);
netdrv_err_t netdrv_listen(netdrv_t *net);
net_queue_t netdrv_accept(netdrv_t *net);