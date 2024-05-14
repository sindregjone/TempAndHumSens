/*
 * http_client.c
 *
 *  Created on: 20. mars 2024
 *      Author: sindregjone
 *      Inspired by https://github.com/SIMS-IOT-Devices/FreeRTOS-ESP-IDF-HTTP-Client/blob/main/proj5.c
 */



#include "http_client.h"
#include "esp_http_client.h"
#include "esp_log.h"
#include "cJSON.h"
#include "string.h"
#include "wifi_client.h"
#include "definitions.h"


esp_err_t client_event_post_handler(esp_http_client_event_handle_t evt)
{
    return ESP_OK;
}


void rest_post(char sensorID[30],float temp, float hum, char batteryLevel[10])
{
	const char *post_data;
    char temp_str[32];
    char hum_str[32];

    snprintf(temp_str, sizeof(temp_str), "%.1f°C", temp);
    snprintf(hum_str, sizeof(hum_str), "%d%%", (int)hum);

	cJSON *root = cJSON_CreateObject();
	cJSON_AddStringToObject(root, "SensorID", sensorID);
	cJSON_AddStringToObject(root, "Temperature", temp_str);
	cJSON_AddStringToObject(root, "Humidity", hum_str);
	cJSON_AddStringToObject(root, "Battery_Level", batteryLevel);


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

    vTaskDelay(100);

    if (err == ESP_OK) {
        ESP_LOGI("HTTP_POST", "HTTP POST Request completed successfully");
    } else {
        ESP_LOGE("HTTP_POST", "HTTP POST Request failed: %s", esp_err_to_name(err));
    }

    esp_http_client_cleanup(client);
    cJSON_Delete(root);
}

