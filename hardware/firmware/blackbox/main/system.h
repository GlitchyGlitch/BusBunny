#include "esp_system.h"

#define esp_free(x) \
  free(x);          \
  x = NULL;

#define esp_panic() esp_restart();