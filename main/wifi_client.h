/*
 * wifi_client.h
 *
 *  
 *  Created by Espressif, edited by sindregjone
 */

#ifndef MAIN_WIFI_CLIENT_H_
#define MAIN_WIFI_CLIENT_H_

#include "esp_event.h"
#include "esp_wifi.h"
#include "esp_blufi.h"
#include "freertos/event_groups.h"

#define EXAMPLE_WIFI_CONNECTION_MAXIMUM_RETRY CONFIG_EXAMPLE_WIFI_CONNECTION_MAXIMUM_RETRY
#define EXAMPLE_INVALID_REASON                255
#define EXAMPLE_INVALID_RSSI                  -128
#define WIFI_LIST_NUM                         10

extern EventGroupHandle_t wifi_event_group;
extern bool gl_sta_connected;
extern bool gl_sta_got_ip;
extern bool ble_is_connected;
extern uint8_t gl_sta_bssid[6];
extern uint8_t gl_sta_ssid[32];
extern int gl_sta_ssid_len;
extern bool gl_sta_is_connecting;

extern esp_blufi_callbacks_t example_callbacks;
extern char* deviceName;

void example_event_callback(esp_blufi_cb_event_t event, esp_blufi_cb_param_t *param);
void ip_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data);
void wifi_event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data);
void initialize_wifi(void);
void deinitialize_wifi(void);


#endif /* MAIN_WIFI_CLIENT_H_ */
