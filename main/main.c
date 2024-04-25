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

#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "freertos/event_groups.h"
#include "freertos/semphr.h"
#include "esp_system.h"
#include "esp_mac.h"
#include "esp_wifi.h"
#include "esp_event.h"
#include "esp_log.h"
#include "nvs_flash.h"
#include "esp_bt.h"
#include "esp_http_client.h"
#include "string.h"
#include "stdbool.h"
#include "esp_bt_main.h"
#include "esp_task_wdt.h"

#include "esp_blufi_api.h"
#include "blufi_example.h"

#include "esp_blufi.h"


#include "driver/i2c.h"
#include "cJSON.h"
#include "time.h"
#include "sys/time.h"
#include "esp_sleep.h"
#include "driver/gpio.h"
#include "driver/adc.h"




//private files includes:
#include "definitions.h"
#include "shtc3_lib.h"
#include "http_client.h"
#include "time_sync.h"
#include "wifi_client.h"
#include "battery_monitor.h"


#define GPIO_SHTC3 7
#define GPIO_BT 4
//#define ADC_PIN ADC_CHANNEL_0
//adc, teller som gjør at den måler og laster opp nivå så og så ofte

#define WAKEUP_TIME_SEC 30
#define WAKEUP_TIME_MIN 15


TaskHandle_t mainTaskHandle = NULL;

uint8_t gatts_service_uuid128_test_X[ESP_UUID_LEN_128] = {0x01, 0xc2, 0xaf, 0x4f, 0xb5, 0x1f, 0x9e, 0x45, 0xcc, 0x8f, 0x4b, 0x91, 0x31, 0xc3, 0xc9, 0xc5};
//gl_profile_tab[PROFILE_X_APP_ID].service_id.id.uuid.len = ESP_UUID_LEN_128;
//memcpy(gl_profile_tab[PROFILE_X_APP_ID].service_id.id.uuid.uuid.uuid128, gatts_service_uuid128_test_X, ESP_UUID_LEN_128);


char Date_Time[100];

uint32_t adcVal = 0;

void adcConfig(void)
{
	adc1_config_width(ADC_WIDTH_BIT_12);
	adc1_config_channel_atten(ADC1_CHANNEL_0,ADC_ATTEN_DB_11);
	adcVal = adc1_get_raw(ADC1_CHANNEL_0);
}

void nvs_init(void)
{
	esp_err_t ret;
	ret = nvs_flash_init();

	if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND)
	{
		ESP_ERROR_CHECK(nvs_flash_erase());
		ret = nvs_flash_init();
	}

	ESP_ERROR_CHECK(ret);
}

void nvs_reinit(void)
{
	esp_err_t ret;
	ret = nvs_flash_erase();
	nvs_init();
	ESP_ERROR_CHECK( ret );
}



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

			vTaskDelay(1500);
			printf("Done setting up BT\r\n");

}


TaskHandle_t bluetoothTaskHandle = NULL;

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


void IRAM_ATTR gpio_isr_handler(void* arg)
    {
    		    // Handle the interrupt event here (e.g., toggle an LED, send a signal)
			BaseType_t xHigherPriorityTaskWoken = pdFALSE;
			vTaskNotifyGiveFromISR(bluetoothTaskHandle, &xHigherPriorityTaskWoken);
			portYIELD_FROM_ISR(xHigherPriorityTaskWoken);
    }


void gpio_button_init() {
    		    gpio_config_t io_conf;
    		    io_conf.intr_type = GPIO_INTR_NEGEDGE; // Trigger on falling edge
    		    io_conf.mode = GPIO_MODE_INPUT;
    		    io_conf.pin_bit_mask = (1ULL << GPIO_BT);
    		    io_conf.pull_up_en = GPIO_PULLUP_ENABLE;
    		    io_conf.pull_down_en = GPIO_PULLDOWN_DISABLE;
    		    gpio_config(&io_conf);

    		    // Install GPIO ISR service
    		    gpio_install_isr_service(ESP_INTR_FLAG_LEVEL1);
    		    // Attach the interrupt service routine
    		    gpio_isr_handler_add(GPIO_BT, gpio_isr_handler, NULL);
    		}






void app_main(void)
{

		mainTaskHandle = xTaskGetCurrentTaskHandle();


	    xTaskCreate(bluetooth_task, "bluetooth_task", 4096, NULL, 2, &bluetoothTaskHandle);

		gpio_button_init();

		gpio_reset_pin(GPIO_SHTC3);
		gpio_set_direction(GPIO_SHTC3, GPIO_MODE_OUTPUT);


	    //config wake-up sources
	    esp_deep_sleep_enable_gpio_wakeup(1 << GPIO_BT, ESP_GPIO_WAKEUP_GPIO_LOW);

		//uint64_t wakeup_time_sec = 15;
		esp_sleep_enable_timer_wakeup(WAKEUP_TIME_MIN * 60 * 1000000ULL);


		esp_sleep_wakeup_cause_t wakeup_reason = esp_sleep_get_wakeup_cause();
		switch(wakeup_reason)
		{
		case ESP_SLEEP_WAKEUP_GPIO:
			printf("Wakeup reason: GPIO\r\n");
			BT_Connect();
			break;

		case ESP_SLEEP_WAKEUP_TIMER:
			printf("Wakeup reason: Timer\r\n");
			break;
		default:
			printf("Not a deepsleep reset\r\n");
		}



		nvs_init(); // Initialize NVS

		initialize_wifi();

		gpio_set_level(GPIO_SHTC3, 1);
		i2c_master_init();

		vTaskDelay(50);

		float temperature, humidity;

		if (read_shtc3(&temperature, &humidity) == ESP_OK) {

			printf("\n Temperature: %.2f C, Humidity: %.2f%% \r\n", temperature, humidity);
			}
		else {
			printf("\n Failed to read temperature and humidity \r\n");
		}

			gpio_set_level(GPIO_SHTC3, 0);

			//Set_SystemTime_SNTP();

			//set_system_time_manually(2028, 2, 29, 12, 00, 00);
			//Get_current_date_time(Date_Time);


			printf("********************************\r\n");
			esp_log_level_set("gpio", ESP_LOG_ERROR); // Only log errors from the GPIO driver

			getBatteryLevel(void);

			printf("Current date and time: %s \n", Date_Time);

			rest_post(temperature, humidity, Date_Time);
			printf("********************************\r\n");
			printf("Entering Deep-Sleep\r\n");

			esp_wifi_stop();
			esp_bt_controller_disable();

			esp_deep_sleep_start();

}
