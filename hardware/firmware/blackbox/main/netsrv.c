#include <stdint.h>
#include "netsrv.h"
#include "esp_log.h"

#include "lwip/sockets.h"
// #include "lwip/sys.h"
// #include <lwip/netdb.h>

static const char *TAG = "netsrv";

netsrv_err_t netsrv_create(netsrv_t *net, ipstr_t ip, uint16_t port)
{
  net->ip = ip;
  // inet_ntoa_r(net->dst_addr.sin_addr, net->ip.bytes, sizeof(ip) - 1);
  net->dst_addr.sin_addr.s_addr = htonl(INADDR_ANY);
  net->dst_addr.sin_family = AF_INET;   // Define address family as Ipv4
  net->dst_addr.sin_port = htons(port); // Define PORT

  net->socket_fd = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (net->socket_fd < 0)
  {
    ESP_LOGE(TAG, "Unable to create socket: errno %d", errno);
    return NETSRV_ERR_CREATE;
  }
  ESP_LOGI(TAG, "Socket created");

  int32_t bind_err = bind(net->socket_fd, (struct sockaddr *)&net->dst_addr, sizeof(net->dst_addr));
  if (bind_err != 0)
  {
    ESP_LOGE(TAG, "Socket unable to bind: errno %d", errno);
    return NETSRV_ERR_BIND;
  }
  ESP_LOGI(TAG, "Socket binded");
  ESP_LOGI(TAG, "IPv4:%s", ip.bytes);
  ESP_LOGI(TAG, "Port:%d", port);
  return NETSRV_OK;
}

netsrv_err_t netsrv_listen(netsrv_t *net)
{
  int32_t listen_err = listen(net->socket_fd, 3);
  if (listen_err != 0)
  {
    ESP_LOGE(TAG, "Error occured during listen: errno %d", errno);
    return NETSRV_ERR_LISTEN
  }
  ESP_LOGI(TAG, "Socket listening");

  return NETSRV_OK;
}

netsrv_err_t netsrv_accept_thread(netsrv_t *net)
{
  struct sockaddr_in client_addr; // Large enough for IPv4
  size_t client_addr_len = sizeof(client_addr);
  sockfd_t client_socket_fd = accept(net->socket_fd, (struct sockaddr *)&client_addr, &client_addr_len);
  if (client_socket_fd < 0)
  {
    ESP_LOGE(TAG, "Unable to accept connection: errno %d", errno);
    return NETSRV_ERR_ACCEPT;
  }
  ESP_LOGI(TAG, "ef rigth bro!");
  return NETSRV_OK;
}