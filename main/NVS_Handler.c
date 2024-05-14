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
	ret = nvs_flash_init(); //initialize the NVS flash

	if (ret == ESP_ERR_NVS_NO_FREE_PAGES || ret == ESP_ERR_NVS_NEW_VERSION_FOUND) //checks if there are no free pages or if a new version i found
	{
		ESP_ERROR_CHECK(nvs_flash_erase()); //erase the flash
		ret = nvs_flash_init(); //reinitialize the NVS flash
	}

	ESP_ERROR_CHECK(ret); //check for errors
}

void nvs_reinit(void)
{
	esp_err_t ret;
	ret = nvs_flash_erase(); //erase the NVS flash
	nvs_init(); //initialize the NVS flash
	ESP_ERROR_CHECK( ret ); //check for errors
}


nvs_handle_t bootCounterHandle; //create handle for bootCounter

uint32_t updateBootCounter(void)
{
	nvs_open("Counter", NVS_READWRITE, &bootCounterHandle); //open NVS handle for the Counter namespace

	uint32_t bootCount = 0;

	nvs_get_u32(bootCounterHandle, "boot_count", &bootCount); //get the current boot count

	bootCount ++; //increment the boot count

	nvs_set_u32(bootCounterHandle, "boot_count", bootCount); //set the new boot count

	nvs_commit(bootCounterHandle); //commit the changes to NVS

	nvs_close(bootCounterHandle); //close the NVS handle

	printf("Boot Count: %ld\n\r", bootCount); //print the updated boot count
	return bootCount; //returns the updated boot count
}

nvs_handle_t batteryLevelHandle; //create handle for batteryLevel


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
