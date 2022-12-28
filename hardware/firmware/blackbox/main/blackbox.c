#include <lwip/sockets.h>
#include <esp_log.h>
#include <string.h>
#include <errno.h>
#include <freertos/FreeRTOS.h>
#include <freertos/task.h>

#include <esp_event.h>
#include <esp_event_loop.h>
#include <esp_system.h>
#include <esp_wifi.h>
#include <nvs_flash.h>
#include <stdio.h>
#include <string.h>
#include "driver/ledc.h"
#include "sdkconfig.h"

#define IFACE0_SSID CONFIG_IFACE0_SSID
#define IFACE0_PASSWORD CONFIG_IFACE0_PASSWORD
#define IFACE0_PORT CONFIG_IFACE0_PORT
#define IFACE0_CHANNEL CONFIG_IFACE0_CHANNEL

static char tag[] = "socket_server";

static void sendData(int socket)
{
  char text[80];
  int i;
  for (i = 1; i <= 10; i++)
  {
    sprintf(text, "Message %d\n", i);
    send(socket, text, strlen(text), 0);
    vTaskDelay(1000 / portTICK_PERIOD_MS);
  }
  close(socket);
}

esp_err_t wifi_event_handler(void *ctx, system_event_t *event)
{ // XXX: Make real event handling
  return ESP_OK;
}

/**

Creates a new Wifi-AP on ESP32

*/
void wifi_start_access_point()
{
  wifi_config_t wifi_config = {
      .ap = {
          .ssid = IFACE0_SSID,
          .password = IFACE0_PASSWORD,
          .channel = IFACE0_CHANNEL,
          .authmode = WIFI_AUTH_WPA2_PSK,
          .max_connection = 1, // TODO: Migrate to menu?
      }};

  tcpip_adapter_init(); // XXX Working
  esp_event_loop_init(wifi_event_handler, NULL);
  wifi_init_config_t wifi_init_config = WIFI_INIT_CONFIG_DEFAULT();
  esp_wifi_init(&wifi_init_config);
  esp_wifi_set_storage(WIFI_STORAGE_RAM);
  esp_wifi_set_mode(WIFI_MODE_AP);
  esp_wifi_set_config(WIFI_IF_AP, &wifi_config);
  esp_wifi_start();
}

/**
 * Create a listening socket.  We then wait for a client to connect.
 * Once a client has connected, we then read until there is no more data
 * and log the data read.  We then close the client socket and start
 * waiting for a new connection.
 */
void socket_server_task()
{
  struct sockaddr_in clientAddress;
  struct sockaddr_in serverAddress;

  // Create a socket that we will listen upon.
  int sock = socket(AF_INET, SOCK_STREAM, IPPROTO_TCP);
  if (sock < 0)
  {
    ESP_LOGE(tag, "socket: %d %s", sock, strerror(errno));
    goto END;
  }

  // Bind our server socket to a port.
  serverAddress.sin_family = AF_INET;
  serverAddress.sin_addr.s_addr = htonl(INADDR_ANY);
  serverAddress.sin_port = htons(IFACE0_PORT);
  int rc = bind(sock, (struct sockaddr *)&serverAddress, sizeof(serverAddress));
  if (rc < 0)
  {
    ESP_LOGE(tag, "bind: %d %s", rc, strerror(errno));
    goto END;
  }

  // Flag the socket as listening for new connections.
  rc = listen(sock, 5);
  if (rc < 0)
  {
    ESP_LOGE(tag, "listen: %d %s", rc, strerror(errno));
    goto END;
  }

  while (1)
  {
    // Listen for a new client connection.
    ESP_LOGD(tag, "Waiting for new client connection");
    socklen_t clientAddressLength = sizeof(clientAddress);
    int clientSock = accept(sock, (struct sockaddr *)&clientAddress, &clientAddressLength);
    if (clientSock < 0)
    {
      ESP_LOGE(tag, "accept: %d %s", clientSock, strerror(errno));
      goto END;
    }
    sendData(clientSock);
  }
END:
  vTaskDelete(NULL);
}

//--------
//  Main
//--------

void app_main()
{
  nvs_flash_init();

  wifi_start_access_point();
  socket_server_task();
  vTaskDelete(NULL);
}
