/*
 * definitions.h
 *
 *  Created on: 20. mars 2024
 *      Author: sindregjone
 */

#ifndef MAIN_DEFINITIONS_H_
#define MAIN_DEFINITIONS_H_

#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

#define CONNECTED_BIT BIT0

#define DEFAULT_SENSOR_ID "defaultSensorID"

#define GPIO_SHTC3 7
#define GPIO_BT 4 //shift back to 4

#define WAKEUP_TIME_SEC 10 //remove
#define WAKEUP_TIME_MIN 1

#define BT_CONNECT_TIME_SEC 30

#define BATTERY_UPDATE_INTERVAL 5 //sets the interval of how often the battery level should be updated


//voltage from battery after voltage divider has range 2.1V - 3.0V
#define BATTERY_HIGH_LEVEL 2700 //approx 3.85V from battery
#define BATTERY_LOW_LEVEL 2400 //approx 3.40V from battery

extern TaskHandle_t mainTaskHandle;


#endif /* MAIN_DEFINITIONS_H_ */
