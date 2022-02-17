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

#define ESP_AP_SSID "BlackBox" // Ssid for ESP32 access point
#define ESP_AP_PASS "BlackBox" // password for ESP32 access point
#define ESP_AP_MAX_CONNECT 1   // Maximum stations that can connect to ESP32
#define PORT 9000

/* FreeRTOS event group to signal when we are connected & ready to make a request */
static EventGroupHandle_t wifi_event_group;

static const char *TAG_AP = "ap";
static const char *TAG = "TCP/IP socket";

int32_t client_socket;
int ip_protocol;
int socket_id;
int bind_err;
int listen_error;

// TODO: switch
static void wifi_event_handler(void *arg, esp_event_base_t event_base,
                               int32_t event_id, void *event_data)
{
  if (event_id == WIFI_EVENT_AP_STACONNECTED)
  {
    wifi_event_ap_staconnected_t *event = (wifi_event_ap_staconnected_t *)event_data;
    ESP_LOGI(TAG, "station " MACSTR " join, AID=%d",
             MAC2STR(event->mac), event->aid);
  }
  else if (event_id == WIFI_EVENT_AP_STADISCONNECTED)
  {
    wifi_event_ap_stadisconnected_t *event = (wifi_event_ap_stadisconnected_t *)event_data;
    ESP_LOGI(TAG, "station " MACSTR " leave, AID=%d",
             MAC2STR(event->mac), event->aid);
  }
}

void wifi_init_ap()
{
  ESP_ERROR_CHECK(esp_netif_init());
  ESP_ERROR_CHECK(esp_event_loop_create_default());
  esp_netif_create_default_wifi_ap();

  wifi_init_config_t cfg = WIFI_INIT_CONFIG_DEFAULT();
  ESP_ERROR_CHECK(esp_wifi_init(&cfg));

  ESP_ERROR_CHECK(esp_event_handler_instance_register(WIFI_EVENT,
                                                      ESP_EVENT_ANY_ID,
                                                      &wifi_event_handler,
                                                      NULL,
                                                      NULL));

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
      &net, (ipstr_t){"0.0.0.0"}, 9000);
  netsrv_listen(&net);
  netsrv_accept_thread(&net);

  // netsrv_accept_thread(&net);
  // for (;;)
  // {
  //   for (;;)
  //   {
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
