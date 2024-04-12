#include "driver/i2c.h"
#include "esp_log.h"
#include <stdio.h>

#define I2C_MASTER_SCL_IO    6    // SCL Pin
#define I2C_MASTER_SDA_IO    5    // SDA Pin
#define I2C_MASTER_NUM       I2C_NUM_0
#define I2C_MASTER_FREQ_HZ   50000
#define SHTC3_SENSOR_ADDR    0x70  // SHTC3 I2C address
#define WRITE_BIT            I2C_MASTER_WRITE
#define READ_BIT             I2C_MASTER_READ
#define ACK_CHECK_EN         0x1
#define ACK_CHECK_DIS        0x0
#define ACK_VAL              0x0
#define NACK_VAL             0x1

// SHTC3 Commands
#define CMD_MEASURE_NMCE      0x7CA2  // Measure in Normal Mode, Clock stretching Enabled, Temp first
#define CMD_MEASURE_LMCE	  0x6458  // Measure in Low power Mode, Clock stretching Enabled, Temp first
#define CMD_MEASURE_LMCD	  0x609C  // Measure in Low power Mode, Clock stretching Enabled, Temp first
#define CMD_SLEEP            0xB098
#define CMD_WAKEUP           0x3517

static const char *TAG = "SHTC3";

/**
 * @brief Calculate CRC8 for SHTC3 data.
 */
uint8_t shtc3_crc_check(uint16_t value, uint8_t crc) {
    const uint8_t POLYNOMIAL = 0x31;
    uint8_t crc_calculated = 0xFF;

    crc_calculated ^= (value >> 8) & 0xFF;
    for (int j = 0; j < 8; j++) {
        if (crc_calculated & 0x80) {
            crc_calculated = (crc_calculated << 1) ^ POLYNOMIAL;
        } else {
            crc_calculated = (crc_calculated << 1);
        }
    }

    crc_calculated ^= value & 0xFF;
    for (int j = 0; j < 8; j++) {
        if (crc_calculated & 0x80) {
            crc_calculated = (crc_calculated << 1) ^ POLYNOMIAL;
        } else {
            crc_calculated = (crc_calculated << 1);
        }
    }

    return crc_calculated == crc;
}

/**
 * @brief Initialize I2C master.
 */
void i2c_master_init() {
    int i2c_master_port = I2C_MASTER_NUM;
    i2c_config_t conf = {
        .mode = I2C_MODE_MASTER,
        .sda_io_num = I2C_MASTER_SDA_IO,
        .scl_io_num = I2C_MASTER_SCL_IO,
        .sda_pullup_en = GPIO_PULLUP_ENABLE,
        .scl_pullup_en = GPIO_PULLUP_ENABLE,
        .master.clk_speed = I2C_MASTER_FREQ_HZ,
    };
    i2c_param_config(i2c_master_port, &conf);
    i2c_driver_install(i2c_master_port, conf.mode, 0, 0, 0);
}

/**
 * @brief Read temperature and humidity from SHTC3.
 */
esp_err_t read_shtc3(float *temperature, float *humidity) {
    uint8_t data[6];
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (SHTC3_SENSOR_ADDR << 1) | WRITE_BIT, ACK_CHECK_EN);
    i2c_master_write_byte(cmd, CMD_WAKEUP >> 8, ACK_CHECK_EN);
    i2c_master_write_byte(cmd, CMD_WAKEUP & 0xFF, ACK_CHECK_EN);
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, 1000 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);

    if (ret != ESP_OK) {
        return ret;
    }

    vTaskDelay(10 / portTICK_PERIOD_MS);  // Wait for sensor to wake up

    cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (SHTC3_SENSOR_ADDR << 1) | WRITE_BIT, ACK_CHECK_EN);
    i2c_master_write_byte(cmd, CMD_MEASURE_LMCE >> 8, ACK_CHECK_EN);
    i2c_master_write_byte(cmd, CMD_MEASURE_LMCE & 0xFF, ACK_CHECK_EN);
    i2c_master_stop(cmd);
    ret = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, 1000 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);

    if (ret!= ESP_OK) {
        return ret;
    }

    // Wait for measurement to complete
    vTaskDelay(100 / portTICK_PERIOD_MS);

    // Read data
    cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (SHTC3_SENSOR_ADDR << 1) | READ_BIT, ACK_CHECK_EN);
    i2c_master_read_byte(cmd, data, ACK_VAL);
    i2c_master_read_byte(cmd, data + 1, ACK_VAL);
    i2c_master_read_byte(cmd, data + 2, ACK_VAL); // CRC for humidity
    i2c_master_read_byte(cmd, data + 3, ACK_VAL);
    i2c_master_read_byte(cmd, data + 4, ACK_VAL);
    i2c_master_read_byte(cmd, data + 5, NACK_VAL); // CRC for temperature
    i2c_master_stop(cmd);
    ret = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, 1000 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);

    if (ret != ESP_OK) {
        return ret;
    }

    // Verify CRC
    if (!shtc3_crc_check((data[0] << 8) | data[1], data[2]) ||
        !shtc3_crc_check((data[3] << 8) | data[4], data[5])) {
        ESP_LOGE(TAG, "CRC check failed");
        return ESP_FAIL;
    }

    // Calculate humidity and temperature
    uint16_t rawTemperature = (data[0] << 8) | data[1];
    uint16_t rawHumidity = (data[3] << 8) | data[4];
    *humidity = (100.0 * rawHumidity) / 65536.0;
    *temperature = -45.0 + ((175.0 * rawTemperature) / 65536.0);

    return ESP_OK;
}

esp_err_t set_shtc3_sleep() {
    i2c_cmd_handle_t cmd = i2c_cmd_link_create();
    i2c_master_start(cmd);
    i2c_master_write_byte(cmd, (SHTC3_SENSOR_ADDR << 1) | WRITE_BIT, ACK_CHECK_EN);
    i2c_master_write_byte(cmd, CMD_SLEEP >> 8, ACK_CHECK_EN);
    i2c_master_write_byte(cmd, CMD_SLEEP & 0xFF, ACK_CHECK_EN);
    i2c_master_stop(cmd);
    esp_err_t ret = i2c_master_cmd_begin(I2C_MASTER_NUM, cmd, 1000 / portTICK_PERIOD_MS);
    i2c_cmd_link_delete(cmd);

    if (ret == ESP_OK) {
        ESP_LOGI(TAG, "SHTC3 set to sleep mode");
    } else {
        ESP_LOGE(TAG, "Failed to set SHTC3 to sleep mode");
    }

    return ret;
}

