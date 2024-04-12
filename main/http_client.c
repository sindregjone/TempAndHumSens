/*
 * http_client.c
 *
 *  Created on: 20. mars 2024
 *      Author: sindregjone
 */

#include "http_client.h"
#include "esp_http_client.h"
#include "esp_log.h"
#include "cJSON.h"
#include <string.h>
#include "wifi_client.h"


esp_err_t client_event_get_handler(esp_http_client_event_handle_t evt)
{
    switch (evt->event_id)
    {
    case HTTP_EVENT_ON_DATA:
        printf("HTTP_EVENT_ON_DATA: %.*s\n", evt->data_len, (char *)evt->data);
        break;

    default:
        break;
    }
    return ESP_OK;
}


void rest_get()
{
    esp_http_client_config_t config_get = {
        .url = "http://10.225.148.229:3000",
        .method = HTTP_METHOD_GET,
        .cert_pem = NULL,
        .event_handler = client_event_get_handler};

    esp_http_client_handle_t client = esp_http_client_init(&config_get);
    esp_http_client_perform(client);
    esp_http_client_cleanup(client);
}


esp_err_t client_event_post_handler(esp_http_client_event_handle_t evt)
{
    return ESP_OK;
}


void rest_post(float temp, float hum, char time[100])
{
	const char *post_data;
    char temp_str[32];
    char hum_str[32];

    snprintf(temp_str, sizeof(temp_str), "%.1f°C", temp);
    snprintf(hum_str, sizeof(hum_str), "%d%%", (int)hum);


	cJSON *root = cJSON_CreateObject();
	cJSON_AddStringToObject(root, "SensorID", "1");
	cJSON_AddStringToObject(root, "Timestamp", time);
	cJSON_AddStringToObject(root, "Temperature", temp_str);
	cJSON_AddStringToObject(root, "Humidity", hum_str);


	post_data = cJSON_Print(root);

    esp_http_client_config_t config_post = {
        .url = "http://10.225.148.229:3000/sendData",
        .method = HTTP_METHOD_POST,
        .event_handler = client_event_post_handler};

    esp_http_client_handle_t client = esp_http_client_init(&config_post);
    esp_http_client_set_header(client, "Content-Type", "application/json");

    printf("Posting JSON: %s\n", post_data);


	esp_http_client_set_post_field(client, post_data, strlen(post_data));
    esp_err_t err = esp_http_client_perform(client);

    if (err == ESP_OK) {
        ESP_LOGI("HTTP_POST", "HTTP POST Request completed successfully");
    } else {
        ESP_LOGE("HTTP_POST", "HTTP POST Request failed: %s", esp_err_to_name(err));
    }

    esp_http_client_cleanup(client);
    cJSON_Delete(root);
}

// http://10.225.148.229:3000
//http://httpbin.org/get
//http://midtskips.no/bac24el04/receive_post.php
