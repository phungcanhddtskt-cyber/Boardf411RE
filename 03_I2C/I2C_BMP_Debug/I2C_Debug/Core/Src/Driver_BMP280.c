/*
 * Driver_BMP280.c
 *
 *  Created on: 3 thg 5, 2026
 *      Author: PhungCanh
 */

#include "Driver_BMP280.h"
#include <stdio.h>
#include <string.h>

static status_dev BMP280_ReadCalibration(bmp280_t *dev)
{
    uint8_t buf[24];

    if ((dev->read_reg)(dev->dev_addr, 0x88, buf, 24) != STATUS_OK)
        return STATUS_ERR;

    dev->dig_T1 = (buf[1] << 8) | buf[0];
    dev->dig_T2 = (buf[3] << 8) | buf[2];
    dev->dig_T3 = (buf[5] << 8) | buf[4];

    dev->dig_P1 = (buf[7] << 8) | buf[6];
    dev->dig_P2 = (buf[9] << 8) | buf[8];
    dev->dig_P3 = (buf[11] << 8) | buf[10];
    dev->dig_P4 = (buf[13] << 8) | buf[12];
    dev->dig_P5 = (buf[15] << 8) | buf[14];
    dev->dig_P6 = (buf[17] << 8) | buf[16];
    dev->dig_P7 = (buf[19] << 8) | buf[18];
    dev->dig_P8 = (buf[21] << 8) | buf[20];
    dev->dig_P9 = (buf[23] << 8) | buf[22];

    return STATUS_OK;
}
status_dev BMP280_Init(bmp280_t *dev)
{
    uint8_t id = 0;
    status_dev stt;
    // 1) Kiểm tra ID
   // if ((dev->read_reg)(dev->dev_addr, BMP280_REG_ID, &id, 1) != STATUS_OK)
    	stt = (dev->read_reg)(dev->dev_addr, BMP280_REG_ID, &id, 1);
    	 printf("sttusid = %d\r\n", stt);
    	 if (stt != STATUS_OK)
    	 {
    	     return STATUS_ERR;
    	 }
     //   return STATUS_ERR;
    printf("ID = 0x%02X\r\n", id);

    if (id != BMP280_ID_VALUE)
        return STATUS_ERR;

    // 2) Reset
    uint8_t reset_cmd = BMP280_RESET_VALUE; // BMP280_RESET_VALUE is not a variable,
    (dev->write_reg)(dev->dev_addr, BMP280_REG_RESET, &reset_cmd, 1);
    (dev->delay)(10);

    // 3) Đọc calibration
    if (BMP280_ReadCalibration(dev) != STATUS_OK)
        return STATUS_ERR;

    // 4) Cấu hình ctrl_meas (osrs_t = x1, osrs_p = x4, normal mode)
    uint8_t ctrl_meas = (1 << 5) | (2 << 2) | 0x03;
    (dev->write_reg)(dev->dev_addr, BMP280_REG_CTRL_MEAS, &ctrl_meas, 1);

    // 5) Cấu hình filter + standby
    uint8_t config = (5 << 5) | (2 << 2);
    (dev->write_reg)(dev->dev_addr, BMP280_REG_CONFIG, &config, 1);

    return STATUS_OK;
}
static int32_t BMP280_ReadTempRaw(bmp280_t *dev)
{
    uint8_t buf[3];
    (dev->read_reg)(dev->dev_addr,BMP280_REG_TEMP_MSB, buf, 3);
    printf("Raw T: %02X %02X %02X\r\n", buf[0], buf[1], buf[2]);
    return (int32_t)((buf[0] << 12) | (buf[1] << 4) | (buf[2] >> 4));
}

float BMP280_ReadTemperature(bmp280_t *dev)
{
    int32_t adc_T = BMP280_ReadTempRaw(dev);

    int32_t var1 = ((((adc_T >> 3) - ((int32_t)dev->dig_T1 << 1))) *
                    ((int32_t)dev->dig_T2)) >> 11;

    int32_t var2 = (((((adc_T >> 4) - ((int32_t)dev->dig_T1)) *
                      ((adc_T >> 4) - ((int32_t)dev->dig_T1))) >> 12) *
                    ((int32_t)dev->dig_T3)) >> 14;

    dev->t_fine = var1 + var2;

    float T = (dev->t_fine * 5 + 128) >> 8;
    return T / 100.0f;
}
static int32_t BMP280_ReadPressRaw(bmp280_t *dev)
{
    uint8_t buf[3];
    (dev->read_reg)(dev->dev_addr, BMP280_REG_PRESS_MSB, buf, 3);
    printf("Raw P: %02X %02X %02X\r\n", buf[0], buf[1], buf[2]);
    return (int32_t)((buf[0] << 12) | (buf[1] << 4) | (buf[2] >> 4));
}

float BMP280_ReadPressure(bmp280_t *dev)
{
    int32_t adc_P = BMP280_ReadPressRaw(dev);

    int64_t var1 = ((int64_t)dev->t_fine) - 128000;
    int64_t var2 = var1 * var1 * (int64_t)dev->dig_P6;
    var2 = var2 + ((var1 * (int64_t)dev->dig_P5) << 17);
    var2 = var2 + (((int64_t)dev->dig_P4) << 35);

    var1 = ((var1 * var1 * (int64_t)dev->dig_P3) >> 8) +
           ((var1 * (int64_t)dev->dig_P2) << 12);
    var1 = (((((int64_t)1) << 47) + var1)) *
           ((int64_t)dev->dig_P1) >> 33;

    if (var1 == 0) return 0; // tránh chia 0

    int64_t p = 1048576 - adc_P;
    p = (((p << 31) - var2) * 3125) / var1;

    var1 = (((int64_t)dev->dig_P9) * (p >> 13) * (p >> 13)) >> 25;
    var2 = (((int64_t)dev->dig_P8) * p) >> 19;

    p = ((p + var1 + var2) >> 8) + (((int64_t)dev->dig_P7) << 4);

    return (float)p / 256.0f; // Pa
}
