#include <string.h>
#include <sys/param.h>
#include <stdint.h>
#include "sdkconfig.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"

#include "netsrv.h"

#include <lwip/netdb.h>

static esp_err_t event_handler(void *ctx, system_event_t *event);
static void tcp_server_task(void *pvParameters);
void wifi_init_ap();
char *getTagValue(char *a_tag_list, char *a_tag);

#define ESP_AP_SSID "BlackBox" // Ssid for ESP32 access point
#define ESP_AP_PASS "BlackBox" // password for ESP32 access point
#define ESP_AP_MAX_CONNECT 1   // Maximum stations that can connect to ESP32
#define PORT 9000

/* FreeRTOS event group to signal when we are connected & ready to make a request */
static EventGroupHandle_t wifi_event_group;

const int IPV4_GOTIP_BIT = BIT0;

static const char *TAG_AP = "ap";
static const char *TAG = "TCP/IP socket";

uint32_t client_socket;
int ip_protocol;
int socket_id;
int bind_err;
int listen_error;

static esp_err_t event_handler(void *ctx, system_event_t *event)
{
  switch (event->event_id)
  {
  case SYSTEM_EVENT_AP_STACONNECTED: // When new stations connects to AP (ESP32), display MAC address and AID
    ESP_LOGI(TAG_AP, "station:" MACSTR " join, AID= %d",
             MAC2STR(event->event_info.sta_connected.mac),
             event->event_info.sta_connected.aid);
    break;

  case SYSTEM_EVENT_AP_STADISCONNECTED: // When stations disconnect from AP (ESP32), display deisconnected stations' MAC and AID
    ESP_LOGI(TAG_AP, "station:" MACSTR "leave, AID= %d",
             MAC2STR(event->event_info.sta_disconnected.mac),
             event->event_info.sta_disconnected.aid);
    // On disconnet, close TCP socket client
    if (client_socket != -1)
    {
      ESP_LOGE(TAG, "Shutting down socket and restarting...");
      shutdown(client_socket, 0);
      close(client_socket);
    }
    break;

  default:
    break;
  }
  return ESP_OK;
}

void wifi_init_ap()
{
  wifi_event_group = xEventGroupCreate(); // Create listener thread

  esp_netif_init();                                          // Initialise lwIP
  ESP_ERROR_CHECK(esp_event_loop_init(event_handler, NULL)); // Start event_handler loop

  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT(); // Create instance of wifi_init_config_t cfg, and assign default values to all members
  ESP_ERROR_CHECK(esp_wifi_init(&cfg));                // Initialise instance of wifi_init_config_t

  wifi_config_t wifi_config_ap = {
      // Set configuration parameters for AP mode
      .ap = {
          .ssid = ESP_AP_SSID,
          .ssid_len = strlen(ESP_AP_SSID),
          .password = ESP_AP_PASS,
          .max_connection = ESP_AP_MAX_CONNECT,
          .authmode = WIFI_AUTH_WPA_WPA2_PSK,
      },
  };

  ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));                      // Set Wifi mode as AP+Station
  ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_AP, &wifi_config_ap)); // Initialise AP configuration
  ESP_LOGI(TAG_AP, "wifi_init_ap finished SSID: %s, password: %s",
           ESP_AP_SSID, ESP_AP_PASS); // Print what credentials the ESP32 is broadcasting as an AP

  ESP_ERROR_CHECK(esp_wifi_start()); // Start the Wifi driver
}

static void tcp_server_task(void *pvParameters)
{
  netsrv_t net;
  netsrv_create(
      &net, (ipstr_t){"0.0.0.0"}, 2000);
  // for (;;)
  // {
  //   for (;;)
  //   {
  //     struct sockaddr_in sourceAddr; // Large enough for IPv4
  //     uint addrLen = sizeof(sourceAddr);
  //     /* Accept connection to incoming client */
  //     client_socket = accept(socket_id, (struct sockaddr *)&sourceAddr, &addrLen);
  //     if (client_socket < 0)
  //     {
  //       ESP_LOGE(TAG, "Unable to accept connection: errno %d", errno);
  //       break;
  //     }
  //     ESP_LOGI(TAG, "Socket accepted");

  //     //Optionally set O_NONBLOCK
  //     //If O_NONBLOCK is set then recv() will return, otherwise it will stall until data is received or the connection is lost.
  //     //fcntl(client_socket,F_SETFL,O_NONBLOCK);

  //     // Clear rx_buffer, and fill with zero's
  //     bzero(rx_buffer, sizeof(rx_buffer));
  //     vTaskDelay(500 / portTICK_PERIOD_MS);
  //     for (;;)
  //     {
  //       ESP_LOGI(TAG, "Waiting for data");
  //       bytes_received = recv(client_socket, rx_buffer, sizeof(rx_buffer) - 1, 0);
  //       ESP_LOGI(TAG, "Received Data");

  //       // Error occured during receiving
  //       if (bytes_received < 0)
  //       {
  //         ESP_LOGI(TAG, "Waiting for data");
  //         vTaskDelay(100 / portTICK_PERIOD_MS);
  //       }
  //       // Connection closed
  //       else if (bytes_received == 0)
  //       {
  //         ESP_LOGI(TAG, "Connection closed");
  //         break;
  //       }
  //       // Data received
  //       else
  //       {
  //         // Get the sender's ip address as string
  //         if (sourceAddr.sin_family == PF_INET)
  //         {
  //           inet_ntoa_r(((struct sockaddr_in *)&sourceAddr)->sin_addr.s_addr, addr_str, sizeof(addr_str) - 1);
  //         }

  //         rx_buffer[bytes_received] = 0; // Null-terminate whatever we received and treat like a string
  //         ESP_LOGI(TAG, "Received %d bytes from %s:", bytes_received, addr_str);
  //         ESP_LOGI(TAG, "%s", rx_buffer);
  //         // TODO: Data handling

  //         // Clear rx_buffer, and fill with zero's
  //         bzero(rx_buffer, sizeof(rx_buffer));
  //       }
  //     }
  //   }
  // }
  vTaskDelete(NULL);
}

void app_main()
{
  ESP_ERROR_CHECK(nvs_flash_init());
  wifi_init_ap();
  xTaskCreate(tcp_server_task, "tcp_server", 4096, NULL, 5, NULL);
}
