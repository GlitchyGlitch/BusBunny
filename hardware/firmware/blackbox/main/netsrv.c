#include <stdint.h>
#include "netsrv.h"
#include "esp_log.h"

#include "lwip/sockets.h"
// #include "lwip/sys.h"
// #include <lwip/netdb.h>

static const char *TAG = "netsrv";

esp_err_t netsrv_create(netsrv_t *net, ipstr_t ip, uint16_t port)
{
  net->ip.bytes = ip;
  inet_ntoa_r(net->dst_addr.sin_addr, net->ip.bytes, sizeof(ip) - 1);
  net->dst_addr.sin_family = AF_INET;   //Define address family as Ipv4
  net->dst_addr.sin_port = htons(port); //Define PORT

  net->socket_id = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (net->socket_id < 0)
  {
    ESP_LOGE(TAG, "Unable to create socket: errno %d", errno);
    return NETSRV_ERR_CREATE;
  }
  ESP_LOGI(TAG, "Socket created");

  int32_t bind_err = bind(net->socket_id, (struct sockaddr *)&net->dst_addr, sizeof(net->dst_addr));
  if (bind_err != 0)
  {
    ESP_LOGE(TAG, "Socket unable to bind: errno %d", errno);
    return NETSRV_ERR_BIND;
  }
  ESP_LOGI(TAG, "Socket binded");
  ESP_LOGI(TAG, "IPv4:%s", ip);
  ESP_LOGI(TAG, "Port:%d", port);
  return NETSRV_OK;
}

esp_err_t netsrv_listen(netsrv_t *net)
{
  int32_t listen_err = listen(net->socket_id, 3);
  if (listen_err != 0)
  {
    ESP_LOGE(TAG, "Error occured during listen: errno %d", errno);
    return NETSRV_ERR_LISTEN
  }
  ESP_LOGI(TAG, "Socket listening");

  return NETSRV_OK
}