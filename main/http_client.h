/*
 * http_client.h
 *
 *  Created on: 20. mars 2024
 *      Author: sindregjone
 */

#ifndef MAIN_HTTP_CLIENT_H_
#define MAIN_HTTP_CLIENT_H_

#include "esp_err.h"
#include "esp_http_client.h"


esp_err_t client_event_get_handler(esp_http_client_event_handle_t evt);
void rest_get(void);
esp_err_t client_event_post_handler(esp_http_client_event_handle_t evt);
void rest_post(char sensorID[30], float temp, float hum, char batteryLevel[10]);


#endif /* MAIN_HTTP_CLIENT_H_ */
