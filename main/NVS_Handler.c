/*
 * nvs.c
 *
 *  Created on: 26. apr. 2024
 *      Author: sindregjone
 */

#include "stdio.h"
#include "stdlib.h"
#include "nvs_flash.h"
#include "esp_log.h"
#include "NVS_Handler.h"
#include "string.h"
#include "definitions.h"

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


nvs_handle_t bootCounterHandle;

uint32_t updateBootCounter(void)
{


	nvs_open("Counter", NVS_READWRITE, &bootCounterHandle);

	uint32_t bootCount = 0;

	nvs_get_u32(bootCounterHandle, "boot_count", &bootCount);

	bootCount ++;

	nvs_set_u32(bootCounterHandle, "boot_count", bootCount);

	nvs_commit(bootCounterHandle);

	nvs_close(bootCounterHandle);

	printf("Boot Count: %ld\n\r", bootCount);
	return bootCount;
}

nvs_handle_t batteryLevelHandle;


void NVSmanageBatteryLevel(int operation, char* batLevel, size_t size)
{

	nvs_open("Battery", NVS_READWRITE, &batteryLevelHandle);

	if(operation == SET_BATTERY_LEVEL)
	{
		nvs_set_str(batteryLevelHandle, "batteryLevel", batLevel);
		nvs_commit(batteryLevelHandle);

		printf("Battery level set to: %s\n\r", batLevel);
	}
	else if(operation == GET_BATTERY_LEVEL)
	{
		nvs_get_str(batteryLevelHandle, "batteryLevel", batLevel, &size);
	}

	nvs_close(batteryLevelHandle);
}

nvs_handle_t SensorIDHandle;

void NVSmanageSensorID(int operation, char* sensorID, size_t size)
{
	nvs_open("ID", NVS_READWRITE, &SensorIDHandle);

		if(operation == SET_SENSOR_ID)
		{
			nvs_set_str(SensorIDHandle, "ID", sensorID);
			nvs_commit(SensorIDHandle);

			printf("SensorID set to: %s\n\r", sensorID);
		}
		else if(operation == GET_SENSOR_ID)
		{
			esp_err_t err = nvs_get_str(SensorIDHandle, "ID", sensorID, &size);

			if(err == ESP_ERR_NVS_NOT_FOUND || size == 0)
			{
				nvs_set_str(SensorIDHandle, "ID", DEFAULT_SENSOR_ID);
			}
			nvs_get_str(SensorIDHandle, "ID", sensorID, &size);
		}

		nvs_close(SensorIDHandle);
}
