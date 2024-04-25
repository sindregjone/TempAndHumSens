/*
 * battery_monitor.c
 *
 *  Created on: 25. apr. 2024
 *      Author: sindregjone
 */

#include "stdio.h"
#include "freertos/FreeRTOS.h"
#include "freertos/task.h"
#include "driver/gpio.h"
#include "sdkconfig.h"
#include "driver/adc.h"
#include "esp_adc_cal.h"
#include "definitions.h"





esp_adc_cal_characteristics_t *adc_chars;

void adcConfig(void)
{
	adc1_config_width(ADC_WIDTH_BIT_12);
	adc1_config_channel_atten(ADC1_CHANNEL_0,ADC_ATTEN_DB_11);

}


void calibrateADC(adc_unit_t unit, adc_atten_t atten, adc_bits_width_t width, uint32_t default_vref)
{

	  if (!adc_chars)
	  {
	        adc_chars = calloc(1, sizeof(esp_adc_cal_characteristics_t));
	  }
	  esp_adc_cal_characterize(unit, atten, width, default_vref, adc_chars);
}

uint32_t getBatteryLevel(void)
{

	uint32_t adcVal = 0;
	uint32_t adcVoltage = 0;

	adcConfig();

	calibrateADC(ADC_UNIT_1, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_12, 1100);

	adcVal = adc1_get_raw(ADC1_CHANNEL_0);
	adcVoltage = esp_adc_cal_raw_to_voltage(adcVal, adc_chars);
	//printf("Battery voltage: %ld mV \r\n", adcVoltage);

	if(BATTERY_HIGH_LEVEL <= adcVoltage)
	{
		printf("Battery condition: High\r\n");
		return 3;
	}

	else if(BATTERY_LOW_LEVEL < adcVoltage && adcVoltage <= BATTERY_HIGH_LEVEL)
	{
		printf("Battery condition: Medium\r\n");
		return 2;
	}

	else if(adcVoltage < BATTERY_LOW_LEVEL)
	{
		printf("Battery condition: Low\r\n");
		return 1;
	}
	else
		printf("Could not get battery condition\r\n");
	return 0;

}
