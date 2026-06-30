/*
 * i2c_bmp280.h
 *
 *  Created on: 1 thg 5, 2026
 *      Author: PhungCanh
 */

#ifndef INC_I2C_BMP280_H_
#define INC_I2C_BMP280_H_
#include "i2c_dev.h"
#include <stdint.h>

#define BMP280_ADDR_LOW     (0x76 << 1)   // address of sensor bmp280
#define BMP280_ADDR_HIGH    (0x77 << 1)

//control register address
#define BMP280_REG_ID        0xD0
#define BMP280_REG_RESET     0xE0
#define BMP280_REG_STATUS    0xF3
#define BMP280_REG_CTRL_MEAS 0xF4
#define BMP280_REG_CONFIG    0xF5
#define BMP280_REG_PRESS_MSB 0xF7

// value use
#define BMP280_RESET_VALUE   0xB6
#define BMP280_ID_VALUE      0x58

typedef struct {
	// temperature
    uint16_t dig_T1;
    int16_t  dig_T2;
    int16_t  dig_T3;

    //pressuar
    uint16_t dig_P1;
    int16_t  dig_P2;
    int16_t  dig_P3;
    int16_t  dig_P4;
    int16_t  dig_P5;
    int16_t  dig_P6;
    int16_t  dig_P7;
    int16_t  dig_P8;
    int16_t  dig_P9;

    int32_t t_fine;
} BMP280_Calib_t;

typedef struct {
    I2C_Dev_t i2c;
    BMP280_Calib_t calib;
} BMP280_t;

HAL_StatusTypeDef BMP280_Init(BMP280_t *dev, I2C_HandleTypeDef *hi2c, uint8_t addr, uint16_t timeout);
float BMP280_ReadTemperature(BMP280_t *dev);
float BMP280_ReadPressure(BMP280_t *dev);
float BMP280_ReadAltitude(BMP280_t *dev, float sea_level_hPa);

#endif /* INC_I2C_BMP280_H_ */
