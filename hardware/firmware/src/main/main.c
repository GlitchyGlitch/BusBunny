#include <sys/param.h>
#include <stdint.h>
#include "sdkconfig.h"

#include "netdrv.h"
#include "system.h"
#include "wifiapdrv.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

#include "esp_system.h"
#include "esp_log.h"
#include "nvs_flash.h"

static const char *TAG = "main"; // unused 
#define SSID CONFIG_WIFI_SSID
#define PASSWORD CONFIG_WIFI_PASSWORD
#define IP CONFIG_WIFI_IP
#define CHANNEL CONFIG_WIFI_CHANNEL
#define PORT CONFIG_WIFI_PORT
#define MAX_CONN CONFIG_WIFI_MAX_CONN

void app_main()
{
  // TODO: make this reconnectable
  ESP_ERROR_CHECK(nvs_flash_init());
  wifiapdrv_create((ssidstr_t){SSID}, (passwdstr_t){PASSWORD}, MAX_CONN);

  netdrv_t net;
  netdrv_create(
      &net, (netdrv_ipstr_t){IP}, PORT, 128);
  netdrv_listen(&net);
  
  netdrv_queue_t queue;
  queue = netdrv_accept(&net);
  ESP_LOGE("DEBUG", "Hello, here i'm;");
  if (queue.err != NETDRV_OK)
  {
    ESP_LOGE("DEBUG", "Panicout");
    esp_panic();
  }

  for (;;)
  {
    netdrv_msg_t msg;
    ESP_LOGE("DEBUG", "In loop");
    BaseType_t ok = xQueueReceive(queue.queue_recv, &msg, 100);
    ESP_LOGE("DEBUG","recv");
    
    if (ok == pdTRUE)
    {
        ESP_LOGE("DEBUG","send");

      xQueueSend(queue.queue_send, &msg, 100);
    } else return;

  }

}
