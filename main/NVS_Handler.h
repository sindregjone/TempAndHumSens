/*
 * nvs.h
 *
 *  Created on: 26. apr. 2024
 *      Author: sindregjone
 */

#ifndef MAIN_NVS_HANDLER_H_
#define MAIN_NVS_HANDLER_H_

#include "stdio.h"

void nvs_init(void);
void nvs_reinit(void);
uint32_t updateBootCounter(void);
void NVSsetBatteryLevel(char *batLevel);
void NVSgetBatteryLevel(char *batLevel, size_t size);


#endif /* MAIN_NVS_HANDLER_H_ */
