/*
 * time_sync.h
 *
 *  Created on: 20. mars 2024
 *      Author: sindregjone
 */

#ifndef MAIN_TIME_SYNC_H_
#define MAIN_TIME_SYNC_H_

#include "sys/time.h"
#include "esp_event.h"

extern char Current_Date_Time[100];

void time_sync_notification_cb(struct timeval *tv);
void event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data);
void Get_current_date_time(char *date_time);
void initialize_sntp(void);
void obtain_time(void);
void Set_SystemTime_SNTP();
void set_system_time_manually(int year, int month, int day, int hour, int min, int sec);

#endif /* MAIN_TIME_SYNC_H_ */
