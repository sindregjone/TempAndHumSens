/*
 * bt.c
 *
 *  Created on: 26. apr. 2024
 *      Author: sindregjone
 *	initialize_bluetooth() is created by Epressif
 */

#include <blufi.h>
#include "stdio.h"
#include "stdlib.h"
#include "string.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "esp_log.h"
#include "esp_bt.h"
#include "esp_blufi_api.h"
#include "definitions.h"
#include "wifi_client.h"
#include "NVS_Handler.h"
#include "BT_Handler.h"

#include "definitions.h"

void initialize_bluetooth() //function for initializing the bluetooth controller
{
	esp_bt_controller_status_t bt_status = esp_bt_controller_get_status(); //get the current bluetooth status
	if(bt_status == ESP_BT_CONTROLLER_STATUS_ENABLED)
	{
		esp_bt_controller_disable(); //disables the bluetooth controller if it is already enabled
	}
	if(bt_status == ESP_BT_CONTROLLER_STATUS_INITED)
	{
		esp_bt_controller_deinit(); //deinitializes the bluetooth controller if it is already initialized
	}

	esp_err_t ret;

	ESP_ERROR_CHECK(esp_bt_controller_mem_release(ESP_BT_MODE_CLASSIC_BT)); //release memory reserced for classic bluetooth

	esp_bt_controller_config_t bt_cfg = BT_CONTROLLER_INIT_CONFIG_DEFAULT(); //initialize default bluetooth controller configuration

	ret = esp_bt_controller_init(&bt_cfg); //initialize bluetooth controller with specified configuration
	if (ret) {
		BLUFI_ERROR("%s initialize bt controller failed: %s\n", __func__, esp_err_to_name(ret));
	}

	ret = esp_bt_controller_enable(ESP_BT_MODE_BLE);
	if (ret) {
		BLUFI_ERROR("%s enable bt controller failed: %s\n", __func__, esp_err_to_name(ret));
		return;
	}


	ret = esp_blufi_host_and_cb_init(&example_callbacks); //initialize bluetooth host and callback functions for BLUFI
	if (ret) {
		BLUFI_ERROR("%s initialise failed: %s\n", __func__, esp_err_to_name(ret));
		return;
	}

	BLUFI_INFO("BLUFI VERSION %04x\n", esp_blufi_get_version()); //print the BLUFI version information
}



void BT_Connect() //function for handling bluetooth connection setup
{
			printf("Entering bluetooth pairing mode\r\n");

			nvs_reinit(); //reinitialize NVS

			deinitialize_wifi(); //deinitialize WiFi

			initialize_wifi(); //initialize WiFi

			initialize_bluetooth(); //initialize bluetooth

			vTaskDelay(BT_CONNECT_TIME_SEC *100); //wait a given time for connecting to a phone
			printf("Done setting up BT\r\n");
}



void bluetooth_task(void *params) //task function for bluetooth operations
{
	while(1){

		printf("Bluetooth task started, waiting for notification...\n");

    		ulTaskNotifyTake(pdTRUE, portMAX_DELAY); // Wait for the notify to be given by the ISR

    		vTaskSuspend(mainTaskHandle); //suspend the main task

    		printf("Bluetooth task: Notification received, proceeding...\n");

            BT_Connect();  // Perform bluetooth connection setup

            printf("Bluetooth task: Setup complete, task ending\n");

            esp_restart(); //restarts the ESP

            vTaskDelete(NULL); //deletes task
	}
}
