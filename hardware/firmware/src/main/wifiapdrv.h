#pragma once

#include "esp_event.h"

typedef struct ssidstr
{
  char bytes[32];
} ssidstr_t;

typedef struct passwdstr
{
  char bytes[64];
} passwdstr_t;

void wifiapdrv_event_handler(void *arg, esp_event_base_t event_base,
                        int32_t event_id, void *event_data);

void wifiapdrv_create(ssidstr_t ssid, passwdstr_t passwd, uint8_t max_conn);