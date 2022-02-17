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
  char rx_buffer[512];    // char array to store received data
  ipstr_t ip;             // char array to store client IP
  int32_t bytes_received; // immediate bytes received
  sockfd_t socket_fd;
  struct sockaddr_in dst_addr;
} netsrv_t;

esp_err_t netsrv_create(netsrv_t *net, ipstr_t ip, uint16_t port);
esp_err_t netsrv_listen(netsrv_t *net);
esp_err_t netsrv_accept_thread(netsrv_t *net);
