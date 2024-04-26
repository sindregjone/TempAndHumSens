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
