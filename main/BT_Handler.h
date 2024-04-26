/*
 * bt.h
 *
 *  Created on: 26. apr. 2024
 *      Author: sindregjone
 */

#ifndef MAIN_BT_HANDLER_H_
#define MAIN_BT_HANDLER_H_


#include "freertos/FreeRTOS.h"
#include "freertos/task.h"

void initialize_bluetooth();
void BT_Connect();
void bluetooth_task(void *params);

extern TaskHandle_t bluetoothTaskHandle;

#endif /* MAIN_BT_HANDLER_H_ */
