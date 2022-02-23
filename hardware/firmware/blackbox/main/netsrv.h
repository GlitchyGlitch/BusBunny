#pragma once
#include "netsrv_err.h"
#include "esp_system.h"
#include "lwip/sockets.h"

typedef int32_t sockfd_t;
typedef int32_t netsrv_err_t;

typedef struct ipstr
{
  char bytes[32];
} ipstr_t;

typedef struct netsrv
{
  size_t rx_buffer_size;
  ipstr_t ip; // char array to store client IP
  sockfd_t sockfd;
  struct sockaddr_in dst_addr;
} netsrv_t;

netsrv_err_t netsrv_create(netsrv_t *net, ipstr_t ip, uint16_t port, size_t rx_buffer_size);
netsrv_err_t netsrv_listen(netsrv_t *net);
void netsrv_accept_task(void *net);
