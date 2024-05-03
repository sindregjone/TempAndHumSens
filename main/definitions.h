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

#define BLUETOOTH_TASK_STACK_SIZE 2048 //sets the bluetooth task stack size
#define BLUETOOTH_TASK_PRIORITY 10 //sets the bluetooth task priority

#define DEFAULT_SENSOR_ID "defaultSensorID" //defines the default name that is used if no custom name is set

#define SERVER_URL "http://10.225.148.229:3000" //defines the server URL

#define GPIO_SHTC3 7 //defines gpio pin for controlling power supply to SHTC3 sensor
#define GPIO_BT 4 //defines gpio pin for bluetooth button

#define WAKEUP_TIME_MIN 1 //sets the sleep time of the uC

#define BT_CONNECT_TIME_SEC 30 //sets the time the uC is in bluetooth connect mode

#define BATTERY_UPDATE_INTERVAL 5 //sets the interval of how often the battery level should be updated


//voltage from battery after voltage divider has range 2.1V - 3.0V
#define BATTERY_HIGH_LEVEL 2700 //approx 3.85V from battery
#define BATTERY_LOW_LEVEL 2400 //approx 3.40V from battery

extern TaskHandle_t mainTaskHandle; //declares an extern handle of the main task


#endif /* MAIN_DEFINITIONS_H_ */
