#include "driver/gpio.h"
#include "driver/can.h"

#include "esp_log.h"

static const char *TAG = "candrv";

candrv_err_t create_driver()
{
  can_general_config_t g_config = CAN_GENERAL_CONFIG_DEFAULT(GPIO_NUM_21, GPIO_NUM_22, CAN_MODE_NORMAL);
  can_timing_config_t t_config = CAN_TIMING_CONFIG_500KBITS();
  can_filter_config_t f_config = CAN_FILTER_CONFIG_ACCEPT_ALL();

  if (can_driver_install(&g_config, &t_config, &f_config) == ESP_OK)
  {
    ESP_LOGI(TAG, "Driver installed");
  }
  else
  {
    ESP_LOGE(TAG, "Failed to install driver");
    return;
  }

  if (can_start() == ESP_OK)
  {
    ESP_LOGI(TAG, "Driver started");
  }
  else
  {
    ESP_LOGE(TAG, "Failed to start driver");
    return;
  }
}