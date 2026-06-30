/*
 * i2c_bmp280.c
 *
 *  Created on: 1 thg 5, 2026
 *      Author: PhungCanh
 */
#include <i2c_bmp280.h>
static HAL_StatusTypeDef BMP280_ReadCalibration(BMP280_t *dev)
{
    uint8_t buf[24];

    if (I2C_ReadReg(&dev->i2c, 0x88, I2C_MEMADD_SIZE_8BIT, buf, 24) != HAL_OK)
        return HAL_ERROR;

    dev->calib.dig_T1 = (buf[1] << 8) | buf[0];
    dev->calib.dig_T2 = (buf[3] << 8) | buf[2];
    dev->calib.dig_T3 = (buf[5] << 8) | buf[4];

    dev->calib.dig_P1 = (buf[7] << 8) | buf[6];
    dev->calib.dig_P2 = (buf[9] << 8) | buf[8];
    dev->calib.dig_P3 = (buf[11] << 8) | buf[10];
    dev->calib.dig_P4 = (buf[13] << 8) | buf[12];
    dev->calib.dig_P5 = (buf[15] << 8) | buf[14];
    dev->calib.dig_P6 = (buf[17] << 8) | buf[16];
    dev->calib.dig_P7 = (buf[19] << 8) | buf[18];
    dev->calib.dig_P8 = (buf[21] << 8) | buf[20];
    dev->calib.dig_P9 = (buf[23] << 8) | buf[22];

    return HAL_OK;
}

HAL_StatusTypeDef BMP280_Init(BMP280_t *dev, I2C_HandleTypeDef *hi2c, uint8_t addr, uint16_t timeout)
{
    dev->i2c.hi2c = hi2c;
    dev->i2c.dev_addr = addr;
    dev->i2c.time_out = timeout;


    uint8_t id = 0;

    // 1) Kiểm tra ID
    if (I2C_ReadReg(&dev->i2c, BMP280_REG_ID, I2C_MEMADD_SIZE_8BIT, &id, 1) != HAL_OK)
        return HAL_ERROR;

    if (id != BMP280_ID_VALUE)
        return HAL_ERROR;

    // 2) Reset
    uint8_t reset_cmd = BMP280_RESET_VALUE; // BMP280_RESET_VALUE is not a variable,
    I2C_WriteReg(&dev->i2c, BMP280_REG_RESET, I2C_MEMADD_SIZE_8BIT, &reset_cmd, 1);
    HAL_Delay(10);

    // 3) Đọc calibration
    if (BMP280_ReadCalibration(dev) != HAL_OK)
        return HAL_ERROR;

    // 4) Cấu hình ctrl_meas (osrs_t = x1, osrs_p = x4, normal mode)
    uint8_t ctrl_meas = (1 << 5) | (2 << 2) | 0x03;
    I2C_WriteReg(&dev->i2c, BMP280_REG_CTRL_MEAS, I2C_MEMADD_SIZE_8BIT, &ctrl_meas, 1);

    // 5) Cấu hình filter + standby
    uint8_t config = (5 << 5) | (2 << 2);
    I2C_WriteReg(&dev->i2c, BMP280_REG_CONFIG, I2C_MEMADD_SIZE_8BIT, &config, 1);

    return HAL_OK;
}

static int32_t BMP280_ReadTempRaw(BMP280_t *dev)
{
    uint8_t buf[3];
    I2C_ReadReg(&dev->i2c, 0xFA, I2C_MEMADD_SIZE_8BIT, buf, 3);
    return (int32_t)((buf[0] << 12) | (buf[1] << 4) | (buf[2] >> 4));
}

float BMP280_ReadTemperature(BMP280_t *dev)
{
    int32_t adc_T = BMP280_ReadTempRaw(dev);

    int32_t var1 = ((((adc_T >> 3) - ((int32_t)dev->calib.dig_T1 << 1))) *
                    ((int32_t)dev->calib.dig_T2)) >> 11;

    int32_t var2 = (((((adc_T >> 4) - ((int32_t)dev->calib.dig_T1)) *
                      ((adc_T >> 4) - ((int32_t)dev->calib.dig_T1))) >> 12) *
                    ((int32_t)dev->calib.dig_T3)) >> 14;

    dev->calib.t_fine = var1 + var2;

    float T = (dev->calib.t_fine * 5 + 128) >> 8;
    return T / 100.0f;
}
static int32_t BMP280_ReadPressRaw(BMP280_t *dev)
{
    uint8_t buf[3];
    I2C_ReadReg(&dev->i2c, BMP280_REG_PRESS_MSB, I2C_MEMADD_SIZE_8BIT, buf, 3);
    return (int32_t)((buf[0] << 12) | (buf[1] << 4) | (buf[2] >> 4));
}

float BMP280_ReadPressure(BMP280_t *dev)
{
    int32_t adc_P = BMP280_ReadPressRaw(dev);

    int64_t var1 = ((int64_t)dev->calib.t_fine) - 128000;
    int64_t var2 = var1 * var1 * (int64_t)dev->calib.dig_P6;
    var2 = var2 + ((var1 * (int64_t)dev->calib.dig_P5) << 17);
    var2 = var2 + (((int64_t)dev->calib.dig_P4) << 35);

    var1 = ((var1 * var1 * (int64_t)dev->calib.dig_P3) >> 8) +
           ((var1 * (int64_t)dev->calib.dig_P2) << 12);
    var1 = (((((int64_t)1) << 47) + var1)) *
           ((int64_t)dev->calib.dig_P1) >> 33;

    if (var1 == 0) return 0; // tránh chia 0

    int64_t p = 1048576 - adc_P;
    p = (((p << 31) - var2) * 3125) / var1;

    var1 = (((int64_t)dev->calib.dig_P9) * (p >> 13) * (p >> 13)) >> 25;
    var2 = (((int64_t)dev->calib.dig_P8) * p) >> 19;

    p = ((p + var1 + var2) >> 8) + (((int64_t)dev->calib.dig_P7) << 4);

    return (float)p / 256.0f; // Pa
}
float BMP280_ReadAltitude(BMP280_t *dev, float sea_level_hPa)
{
    float pressure_hPa = BMP280_ReadPressure(dev) / 100.0f;
    return 44330.0f * (1.0f - powf(pressure_hPa / sea_level_hPa, 0.1903f));
}

