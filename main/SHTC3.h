/*
 * SHTC3.h
 *
 *  Created on: 12. mars 2024
 *      Author: sindregjone
 */

#ifndef MAIN_SHTC3_H_
#define MAIN_SHTC3_H_

uint8_t shtc3_crc_check(uint16_t value, uint8_t crc);
void i2c_master_init();
esp_err_t read_shtc3(float *temperature, float *humidity);
esp_err_t set_shtc3_sleep();

#endif /* MAIN_SHTC3_H_ */
