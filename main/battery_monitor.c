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
#include "string.h"




esp_adc_cal_characteristics_t *adc_chars; //declare a pointer for ADC calibration characteristics

void adcConfig(void) //function to configure the ADC
{
	adc1_config_width(ADC_WIDTH_BIT_12); //set the ADC width to 12 bits
	adc1_config_channel_atten(ADC1_CHANNEL_0,ADC_ATTEN_DB_11); //set the attenuation for ADC channel 0 to 11dB

}


void calibrateADC(adc_unit_t unit, adc_atten_t atten, adc_bits_width_t width, uint32_t default_vref) //function to calibrate the ADC
{

	  if (!adc_chars)
	  {
	        adc_chars = calloc(1, sizeof(esp_adc_cal_characteristics_t)); //allocate memory for ADC characteristics
	  }
	  esp_adc_cal_characterize(unit, atten, width, default_vref, adc_chars); //calibrates the ADC with the provided parameters
}

void getBatteryLevel(char *batLevel) //function that measure the voltage of battery using the ADC
{

	uint32_t adcVal = 0; //declares variable to hold the raw value from ADC
	uint32_t adcVoltage = 0; //declares variable to hold calculated voltage level from ADC value

	adcConfig(); //configure ADC

	calibrateADC(ADC_UNIT_1, ADC_ATTEN_DB_11, ADC_WIDTH_BIT_12, 1100); //calibrate the ADC

	adcVal = adc1_get_raw(ADC1_CHANNEL_0); //get raw ADC value from channel 0
	adcVoltage = esp_adc_cal_raw_to_voltage(adcVal, adc_chars); //convert the raw ADC value to voltage
	printf("Measured battery voltage: %ld mV \r\n", adcVoltage);

	if(BATTERY_HIGH_LEVEL <= adcVoltage)
	{
		printf("Measured battery level: High\r\n");
		strcpy(batLevel, "HIGH"); //set the battery level to HIGH
	}

	else if(BATTERY_LOW_LEVEL < adcVoltage && adcVoltage <= BATTERY_HIGH_LEVEL)
	{
		printf("Measured battery level: Medium\r\n");
		strcpy(batLevel, "MEDIUM"); //set the battery level to MEDIUM
	}

	else if(adcVoltage < BATTERY_LOW_LEVEL)
	{
		printf("Measured battery level: Low\r\n");
		strcpy(batLevel, "LOW"); //set the battery level to LOW
	}
	else
		printf("Could not get battery condition\r\n");

}
