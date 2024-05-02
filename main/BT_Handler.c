/*
 * bt.c
 *
 *  Created on: 26. apr. 2024
 *      Author: sindregjone
 */

#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_bt.h"
#include "esp_blufi_api.h"
#include "blufi_example.h"

//private files includes:
#include "definitions.h"
#include "wifi_client.h"
#include "NVS_Handler.h"
#include "BT_Handler.h"
#include "definitions.h"

void initialize_bluetooth()
{
	esp_bt_controller_status_t bt_status = esp_bt_controller_get_status();
	if(bt_status == ESP_BT_CONTROLLER_STATUS_ENABLED)
	{
		esp_bt_controller_disable();
	}
	if(bt_status == ESP_BT_CONTROLLER_STATUS_INITED)
	{
		esp_bt_controller_deinit();
	}

	esp_err_t ret;

	ESP_ERROR_CHECK(esp_bt_controller_mem_release(ESP_BT_MODE_CLASSIC_BT));

	esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT();

	ret = esp_bt_controller_init(&bt_cfg);
	if (ret) {
		BLUFI_ERROR("%s initialize bt controller failed: %s\n", __func__, esp_err_to_name(ret));
	}

	ret = esp_bt_controller_enable(ESP_BT_MODE_BLE);
	if (ret) {
		BLUFI_ERROR("%s enable bt controller failed: %s\n", __func__, esp_err_to_name(ret));
		return;
	}


	ret = esp_blufi_host_and_cb_init(&example_callbacks);
	if (ret) {
		BLUFI_ERROR("%s initialise failed: %s\n", __func__, esp_err_to_name(ret));
		return;
	}

	BLUFI_INFO("BLUFI VERSION %04x\n", esp_blufi_get_version());
}



void BT_Connect()
{
			printf("Entering bluetooth pairing mode\r\n");

			nvs_reinit();

			deinitialize_wifi();

			initialize_wifi();

			initialize_bluetooth();

			vTaskDelay(BT_CONNECT_TIME_SEC *100);
			printf("Done setting up BT\r\n");
}



void bluetooth_task(void *params)
{
	while(1){

		printf("Bluetooth task started, waiting for notification...\n");
        // Wait for the notify to be given by the ISR

    		ulTaskNotifyTake(pdTRUE, portMAX_DELAY);

    		vTaskSuspend(mainTaskHandle);

    		printf("Bluetooth task: Notification received, proceeding...\n");

            BT_Connect();  // Perform connection setup

            printf("Bluetooth task: Setup complete, task ending.\n");
            //vTaskDelay(100);
            esp_restart();
            //vTaskResume(mainTaskHandle);
            vTaskDelete(NULL);
	}
}
