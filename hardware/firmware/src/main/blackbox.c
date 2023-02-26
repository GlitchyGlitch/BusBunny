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

static const char *TAG = "main";
#define SSID CONFIG_WIFI_SSID
#define PASSWORD CONFIG_WIFI_PASSWORD
#define IP "10.0.0.1"
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
      &net, (ipstr_t){IP}, PORT, 128);
  netdrv_listen(&net);

  net_queue_t queue;
  char rx_buffer[200]; // TODO: chceck size
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
    {
      xQueueSend(queue.queue_send, (void *)&rx_buffer, 10);
    }

    // TODO: program is stuck here
  }
}
