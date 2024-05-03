/*
 * SPDX-FileCopyrightText: 2021-2022 Espressif Systems (Shanghai) CO LTD
 *
 * SPDX-License-Identifier: Unlicense OR CC0-1.0
 */


/****************************************************************************
* This is a demo for bluetooth config wifi connection to ap. You can config ESP32 to connect a softap
* or config ESP32 as a softap to be connected by other device. APP can be downloaded from github
* android source code: https://github.com/EspressifApp/EspBlufi
* iOS source code: https://github.com/EspressifApp/EspBlufiForiOS
****************************************************************************/

#include "driver/gpio.h"
#include "driver/adc.h"
#include "esp_system.h"
#include "esp_mac.h"
#include "esp_log.h"
#include "esp_bt.h"
#include "esp_sleep.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "stdio.h"
#include "stdlib.h"
#include "string.h"


//private files includes:
#include "battery_monitor.h"
#include "BT_Handler.h"
#include "definitions.h"
#include "http_client.h"
#include "NVS_Handler.h"
#include "SHTC3.h"
#include "wifi_client.h"




//uint8_t gatts_service_uuid128_test_X[ESP_UUID_LEN_128] = {0x01, 0xc2, 0xaf, 0x4f, 0xb5, 0x1f, 0x9e, 0x45, 0xcc, 0x8f, 0x4b, 0x91, 0x31, 0xc3, 0xc9, 0xc5};
//gl_profile_tab[PROFILE_X_APP_ID].service_id.id.uuid.len = ESP_UUID_LEN_128;
//memcpy(gl_profile_tab[PROFILE_X_APP_ID].service_id.id.uuid.uuid.uuid128, gatts_service_uuid128_test_X, ESP_UUID_LEN_128);


TaskHandle_t mainTaskHandle = NULL; //initialize the task handle for the main task to NULL
TaskHandle_t bluetoothTaskHandle = NULL; //initialize the task handle for the bluetooth task to NULL

void IRAM_ATTR gpio_isr_handler(void* arg)
    {
      // Handle the interrupt event here (e.g., toggle an LED, send a signal)
		 BaseType_t xHigherPriorityTaskWoken = pdFALSE;
		 vTaskNotifyGiveFromISR(bluetoothTaskHandle, &xHigherPriorityTaskWoken);
		 portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }


void gpio_BT_button_init()
{
    	gpio_config_t io_conf;

      	io_conf.intr_type = GPIO_INTR_NEGEDGE; // Trigger on falling edge
    	io_conf.mode = GPIO_MODE_INPUT;
   	    io_conf.pin_bit_mask = (1ULL << GPIO_BT);
 	    io_conf.pull_up_en = GPIO_PULLUP_ENABLE;
        io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;

 	    gpio_config(&io_conf);
    	gpio_install_isr_service(ESP_INTR_FLAG_LEVEL1);// Install GPIO ISR service
        gpio_isr_handler_add(GPIO_BT, gpio_isr_handler, NULL);// Attach the interrupt service routine
}

void app_main(void)
{
		char batteryLevel[10];
		char deviceName[30];
		float temperature, humidity;

		mainTaskHandle = xTaskGetCurrentTaskHandle(); //get the task handle from mainTask

	    xTaskCreate(bluetooth_task, "bluetooth_task", BLUETOOTH_TASK_STACK_SIZE, NULL, BLUETOOTH_TASK_PRIORITY, &bluetoothTaskHandle); //starts the bluetooth task

	    gpio_BT_button_init(); //initializes the BT button

		gpio_reset_pin(GPIO_SHTC3); //resets the SHTC3_GPIO
		gpio_set_direction(GPIO_SHTC3, GPIO_MODE_OUTPUT); //Sets the GPIO_SHTC3 as an output

	    //config wake-up sources:
	    esp_deep_sleep_enable_gpio_wakeup(1 << GPIO_BT, ESP_GPIO_WAKEUP_GPIO_LOW); //wake up source that wakes up the uC when a button is pressed

		esp_sleep_enable_timer_wakeup(WAKEUP_TIME_MIN * 60 * 1000000ULL); //wake up source that wakes up the uC after a given time


		esp_sleep_wakeup_cause_t wakeup_reason = esp_sleep_get_wakeup_cause(); //gets the reason why the uC was woken up
		switch(wakeup_reason) //check what the reason for wakeup is
		{
		case ESP_SLEEP_WAKEUP_GPIO:
			printf("Wakeup reason: GPIO\r\n");
			BT_Connect(); //starts the bluetooth connection for setting up wifi
			break;

		case ESP_SLEEP_WAKEUP_TIMER:
			printf("Wakeup reason: Timer\r\n");
			break;
		default:
			printf("Not a deepsleep reset\r\n");
		}

		nvs_init(); // Initialize NVS
		vTaskDelay(100);

		initialize_wifi(); //initializes wifi
		vTaskDelay(100);

		gpio_set_level(GPIO_SHTC3, 1); //turns the SHTC3 sensor ON
		i2c_master_init(); //initialize i2c

		if (read_shtc3(&temperature, &humidity) != ESP_OK) //get the temp and hum from SHTC3 sensor
		{
			printf("\n Failed to read temperature and humidity \r\n");
		}

			gpio_set_level(GPIO_SHTC3, 0); //turns the SHTC3 sensor OFF


			esp_log_level_set("gpio", ESP_LOG_ERROR); // Only log errors from the GPIO driver

			uint32_t localBootCount = updateBootCounter(); //updates the boot counter

			NVSmanageBatteryLevel(GET_BATTERY_LEVEL, batteryLevel, sizeof(batteryLevel)); //gets the last measured battery level from NVS

			if((localBootCount % BATTERY_UPDATE_INTERVAL) == 0 || localBootCount == 1) //checks if it is time for measuring the battery level
			{
				printf("Updating battery Level\r\n");
				getBatteryLevel(batteryLevel); //measures the battery level with ADC
				NVSmanageBatteryLevel(SET_BATTERY_LEVEL, batteryLevel, sizeof(batteryLevel)); //updates the battery level
			}


			NVSmanageSensorID(GET_SENSOR_ID, deviceName, sizeof(deviceName)); //gets the SensorID from NVS

			rest_post(deviceName, temperature, humidity, batteryLevel); //post the JSON file to the server

			printf("Entering Deep-Sleep\r\n");

			esp_wifi_stop(); //stops wifi
			esp_bt_controller_disable(); //disables the bt controller

			esp_deep_sleep_start(); //sends the uC into deep sleep

}


