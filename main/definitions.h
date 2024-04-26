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

#define BATTERY_HIGH_LEVEL 2000
#define BATTERY_LOW_LEVEL 1000


extern TaskHandle_t mainTaskHandle;


#endif /* MAIN_DEFINITIONS_H_ */
