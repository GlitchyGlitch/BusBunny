#pragma once
#include "netdrv_err.h"
#include "esp_system.h"
#include "lwip/sockets.h"

//NOTE: Max size of message for this implementation
#define MESSAGE_SIZE 255
#define RAW_MESSAGE_SIZE 256

typedef int32_t sockfd_t;
typedef int32_t netdrv_err_t;
typedef uint16_t port_t;

typedef struct ipstr
{
  char bytes[32];
} ipstr_t;

typedef struct netdrv
{
  size_t rx_buffer_size;
  ipstr_t ip;
  sockfd_t sockfd;
  struct sockaddr_in dst_addr;
} netdrv_t;

typedef struct net_queue
{
  QueueHandle_t queue_recv;
  QueueHandle_t queue_send;
  netdrv_err_t err;
} net_queue_t;

typedef struct net_msg
{
  size_t size;
  char data[MESSAGE_SIZE];
} net_msg_t;

typedef char net_raw_msg_t[RAW_MESSAGE_SIZE];
net_raw_msg_t *netdrv_serialize_msg(net_msg_t msg);
netdrv_err_t netdrv_create(netdrv_t *net, ipstr_t ip, port_t port, size_t rx_buffer_size);
netdrv_err_t netdrv_listen(netdrv_t *net);
net_queue_t netdrv_accept(netdrv_t *net);