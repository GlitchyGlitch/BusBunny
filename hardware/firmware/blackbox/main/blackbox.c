#include <string.h>
#include <sys/param.h>
#include <stdint.h>
#include "sdkconfig.h"

#include "netdrv.h"
#include "system.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

#include "esp_system.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"

#include <lwip/netdb.h>

#define ESP_AP_SSID "BlackBox" // Ssid for ESP32 access point
#define ESP_AP_PASS "BlackBox" // password for ESP32 access point
#define ESP_AP_MAX_CONNECT 1   // Maximum stations that can connect to ESP32
#define PORT 9000

static const char *TAG = "wifi_ap";

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

  ESP_ERROR_CHECK(esp_wifi_set_mode(WIFI_MODE_AP));
  ESP_ERROR_CHECK(esp_wifi_set_config(ESP_IF_WIFI_AP, &wifi_config_ap));
  ESP_LOGI(TAG, "AccessPoint initialized");

  ESP_ERROR_CHECK(esp_wifi_start()); // Start the Wifi driver
}
void app_main()
{
  ESP_ERROR_CHECK(nvs_flash_init());
  wifi_init_ap();
  netdrv_t net;
  netdrv_create(
      &net, (ipstr_t){"0.0.0.0"}, 9000, 128);
  netdrv_listen(&net);
  for (;;)
  {
    net_queue_t queue;
    char rx_buffer[200];
    queue = netdrv_accept(&net);
    if (queue.err != NETDRV_OK)
    {
      esp_panic();
    }
    // TODO: message as struct
    for (;;)
    {
      BaseType_t ok = xQueueReceive(queue.queue_recv, &rx_buffer, (TickType_t)10);
      if (ok == pdTRUE)
        ESP_LOGI(TAG, "recv: %s", rx_buffer);
    }
  }
}
