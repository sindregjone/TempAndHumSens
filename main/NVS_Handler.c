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

void NVSsetBatteryLevel(char *batLevel)
{

	nvs_open("Battery", NVS_READWRITE, &batteryLevelHandle);

	//nvs_set_u32(batteryLevelHandle, "batteryLevel", batteryLevel);
	nvs_set_str(batteryLevelHandle, "batteryLevel", batLevel);

	nvs_commit(batteryLevelHandle);

	nvs_close(batteryLevelHandle);

	printf("Battery Level: %s\n\r", batLevel);
	//return batLevel;
}

void NVSgetBatteryLevel(char *batLevel, size_t size)
{
	//char batLevel[20];

	nvs_open("Battery", NVS_READWRITE, &batteryLevelHandle);

	nvs_get_str(batteryLevelHandle, "batteryLevel", batLevel, &size);

	nvs_close(batteryLevelHandle);

	//return batLevel;

}
