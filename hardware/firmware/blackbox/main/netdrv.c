

esp_err_t wifi_start_access_point(ssid *char, int )
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
