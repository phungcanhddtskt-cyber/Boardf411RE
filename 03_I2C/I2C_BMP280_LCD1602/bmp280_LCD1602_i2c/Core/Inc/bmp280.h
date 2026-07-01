/*
 * bmp280.h
 *
 *  Created on: 3 thg 6, 2026
 *      Author: PhungCanh
 */

#ifndef INC_BMP280_H_
#define INC_BMP280_H_

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
#define BMP280_REG_TEMP_MSB 0xFA
// value use
#define BMP280_RESET_VALUE   0xB6
#define BMP280_ID_VALUE      0x58

typedef enum{
	STATUS_OK = 0,
	STATUS_ERR,
	STATUS_BUSY,
	STATUS_TIMEOUT
}status_dev;

typedef struct {

	status_dev (*write_reg)(uint8_t dev_addr, uint8_t reg, uint8_t *buff,uint16_t len);
	status_dev (*read_reg)(uint8_t dev_addr, uint8_t reg, uint8_t *buff,uint16_t len);
	void (*delay)(uint32_t ms);

	uint8_t  dev_addr;

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
} bmp280_t;

status_dev BMP280_Init(bmp280_t *dev);
float BMP280_ReadTemperature(bmp280_t *dev);
float BMP280_ReadPressure(bmp280_t *dev);

#endif /* INC_BMP280_H_ */
