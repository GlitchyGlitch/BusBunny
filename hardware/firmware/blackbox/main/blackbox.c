#include <sys/param.h>
#include <stdint.h>
#include "sdkconfig.h"

#include "netdrv.h"
#include "system.h"
#include "wifi.h"

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"

#include "esp_system.h"
#include "esp_log.h"
#include "nvs_flash.h"

static const char *TAG = "main";
volatile bool is_connected = false;
#define PORT 9000

void app_main()
{
  // TODO: make this reconnectable
  ESP_ERROR_CHECK(nvs_flash_init());
  wifi_init_ap();

  netdrv_t net;
  netdrv_create(
      &net, (ipstr_t){"0.0.0.0"}, PORT, 128);
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
