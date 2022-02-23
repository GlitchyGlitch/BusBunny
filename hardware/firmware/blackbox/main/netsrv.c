#include <stdint.h>
#include "netsrv.h"
#include "esp_log.h"

#include "lwip/sockets.h"
// #include "lwip/sys.h"
// #include <lwip/netdb.h>

static const char *TAG = "netsrv";

netsrv_err_t netsrv_create(netsrv_t *net, ipstr_t ip, uint16_t port, size_t rx_buffer_size)
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
    return NETSRV_ERR_CREATE;
  }
  ESP_LOGI(TAG, "Socket created");

  int32_t opt = 1;
  setsockopt(net->sockfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
  ESP_LOGI(TAG, "Socket configured");

  int32_t bind_err = bind(net->sockfd, (struct sockaddr *)&net->dst_addr, sizeof(net->dst_addr));
  if (bind_err != 0)
  {
    ESP_LOGE(TAG, "Socket unable to bind: errno %d", errno);
    return NETSRV_ERR_BIND;
  }
  ESP_LOGI(TAG, "Socket bound");
  ESP_LOGI(TAG, "IPv4: %s", ip.bytes);
  ESP_LOGI(TAG, "Port: %d", port);
  return NETSRV_OK;
}

netsrv_err_t netsrv_listen(netsrv_t *net)
{
  int32_t listen_err = listen(net->sockfd, 1);
  if (listen_err != 0)
  {
    ESP_LOGE(TAG, "Error occured during listen: errno %d", errno);
  }
  ESP_LOGI(TAG, "Socket listening");

  return NETSRV_OK;
}

static void handle_proto(sockfd_t client_sockfd)
{
  size_t rx_len;
  char rx_buffer[128];

  do
  {
    rx_len = recv(client_sockfd, rx_buffer, sizeof(rx_buffer) - 1, 0);
    if (rx_len < 0)
    {
      ESP_LOGE(TAG, "Error occurred during receiving: errno %d", errno);
    }
    else if (rx_len == 0)
    {
      ESP_LOGW(TAG, "Connection closed");
    }
    else
    {
      rx_buffer[rx_len] = 0;
      ESP_LOGI(TAG, "Received %d bytes: %s", rx_len, rx_buffer);
    }
  } while (rx_len > 0);
}

void netsrv_accept_task(void *netsrv)
{
  netsrv_t *net;
  net = (netsrv_t *)netsrv;
  for (;;)
  {
    struct sockaddr_in client_addr;
    size_t client_addr_len = sizeof(client_addr);
    sockfd_t client_sockfd = accept(net->sockfd, (struct sockaddr *)&client_addr, &client_addr_len);
    if (client_sockfd < 0)
    {
      ESP_LOGE(TAG, "Unable to accept connection: errno %d", errno);
      break;
    }

    handle_proto(client_sockfd); // TODO: pass with netsrv
    shutdown(client_sockfd, 0);
    close(client_sockfd);
  }
}
