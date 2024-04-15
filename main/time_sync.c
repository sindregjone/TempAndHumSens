/*
 * time_sync.c
 *
 *  Created on: 20. mars 2024
 *      Author: sindregjone
 */

#include "time_sync.h"
#include "freertos/FreeRTOS.h"
#include "freertos/event_groups.h"
#include "definitions.h"
#include "esp_event.h"
#include "esp_wifi.h"
#include "esp_sntp.h"
#include "sys/time.h"

extern EventGroupHandle_t wifi_event_group;


char Current_Date_Time[100];


void time_sync_notification_cb(struct timeval *tv)
{
	printf("Notification of a time synchronization event");
}



void event_handler(void* arg, esp_event_base_t event_base, int32_t event_id, void* event_data)
{
    if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_START)
	{
        esp_wifi_connect();
    } else if (event_base == WIFI_EVENT && event_id == WIFI_EVENT_STA_DISCONNECTED)
	{
       // if (s_retry_num < EXAMPLE_ESP_MAXIMUM_RETRY)
		//{
            esp_wifi_connect();
           // s_retry_num++;
            printf("retry to connect to the AP");
        //} else {
        //    xEventGroupSetBits(s_wifi_event_group, WIFI_FAIL_BIT);
        printf("connect to the AP fail");
    } else if (event_base == IP_EVENT && event_id == IP_EVENT_STA_GOT_IP) {
        ip_event_got_ip_t* event = (ip_event_got_ip_t*) event_data;
        printf("got ip:" IPSTR, IP2STR(&event->ip_info.ip));

        //s_retry_num = 0;
        xEventGroupSetBits(wifi_event_group, CONNECTED_BIT);
    }
}

void Get_current_date_time(char *date_time){
	char strftime_buf[64];
	time_t now;
	    struct tm timeinfo;
	    time(&now);
	    localtime_r(&now, &timeinfo);

	    	// Set timezone to Norwegian Standard Time
	    	    setenv("TZ", "CET-1CEST,M3.5.0,M10.5.0/3", 1);
	    	    tzset();
	    	    localtime_r(&now, &timeinfo);

	    	    strftime(strftime_buf, sizeof(strftime_buf), "%c", &timeinfo);
	    	    printf("The current date/time is: %s \n", strftime_buf);
                strcpy(date_time,strftime_buf);
}


void initialize_sntp(void)
{
    printf("Initializing SNTP \n");
    sntp_setoperatingmode(SNTP_OPMODE_POLL);
    sntp_setservername(0, "pool.ntp.org");
    sntp_set_time_sync_notification_cb(time_sync_notification_cb);
#ifdef CONFIG_SNTP_TIME_SYNC_METHOD_SMOOTH
    sntp_set_sync_mode(SNTP_SYNC_MODE_SMOOTH);
#endif
    sntp_init();
}
void obtain_time(void)
{
    initialize_sntp();
    // wait for time to be set
    time_t now = 0;
    struct tm timeinfo = { 0 };
    int retry = 0;
    const int retry_count = 5;
    while (sntp_get_sync_status() == SNTP_SYNC_STATUS_RESET && ++retry < retry_count) {
    	printf("Waiting for system time to be set... (%d/%d)\n", retry, retry_count);
        vTaskDelay(2000 / portTICK_PERIOD_MS);
    }
    time(&now);
    localtime_r(&now, &timeinfo);
}
 void Set_SystemTime_SNTP()  {

	 time_t now;
	    struct tm timeinfo;
	    time(&now);
	    localtime_r(&now, &timeinfo);
	    // Is time set? If not, tm_year will be (1970 - 1900).
	    if (timeinfo.tm_year < (2016   - 1900)) {
	    	printf("Time is not set yet. Connecting to WiFi and getting time over NTP.\n");
	        obtain_time();
	        // update 'now' variable with current time
	        time(&now);
	    }
}


 void set_system_time_manually(int year, int month, int day, int hour, int min, int sec) {
     struct tm timeinfo = {0};
     timeinfo.tm_year = year - 1900; // År siden 1900
     timeinfo.tm_mon = month - 1; // Måned, start fra 0
     timeinfo.tm_mday = day; // Dag i måneden
     timeinfo.tm_hour = hour;
     timeinfo.tm_min = min;
     timeinfo.tm_sec = sec;

     time_t t = mktime(&timeinfo);
     struct timeval now = { .tv_sec = t };
     settimeofday(&now, NULL);

     printf("Tid manuelt satt til: %s", asctime(&timeinfo));
 }

