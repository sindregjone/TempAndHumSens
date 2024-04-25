/*
 * battery_monitor.h
 *
 *  Created on: 25. apr. 2024
 *      Author: sindregjone
 */

#ifndef MAIN_BATTERY_MONITOR_H_
#define MAIN_BATTERY_MONITOR_H_

void adcConfig(void);
void calibrateADC(adc_unit_t unit, adc_atten_t atten, adc_bits_width_t width, uint32_t default_vref);
void getBatteryLevel(esp_adc_cal_characteristics_t adc_chars);


#endif /* MAIN_BATTERY_MONITOR_H_ */
