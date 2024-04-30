/*
 * nvs.h
 *
 *  Created on: 26. apr. 2024
 *      Author: sindregjone
 */

#ifndef MAIN_NVS_HANDLER_H_
#define MAIN_NVS_HANDLER_H_


#define SET_BATTERY_LEVEL 0
#define GET_BATTERY_LEVEL 1

#define SET_SENSOR_ID 0
#define GET_SENSOR_ID 1

#include "stdio.h"

void nvs_init(void);
void nvs_reinit(void);
uint32_t updateBootCounter(void);
void NVSmanageBatteryLevel(int operation, char* batLevel, size_t size);
void NVSmanageSensorID(int operation, char* sensorID, size_t size);


#endif /* MAIN_NVS_HANDLER_H_ */
