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
uint32_t getBatteryLevel(void);


#endif /* MAIN_BATTERY_MONITOR_H_ */
