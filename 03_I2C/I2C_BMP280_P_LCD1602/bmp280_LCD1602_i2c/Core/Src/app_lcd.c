/*
 * app_lcd.c
 *
 *  Created on: 31 thg 5, 2026
 *      Author: PhungCanh
 */


#include "app_lcd.h"
#include "lcd1602_pcf8574.h"
#include "i2c.h"
#include "bmp280.h"
#include "fl_to_str.h"
bmp280_t bmp;
static int stm32_i2c_write(uint8_t addr,
                           const uint8_t *buf,
                           uint16_t len,
                           void *user_ctx)
{
    I2C_HandleTypeDef *hi2c = (I2C_HandleTypeDef *)user_ctx;
    return (HAL_I2C_Master_Transmit(hi2c, addr << 1, (uint8_t*)buf, len, 100) == HAL_OK) ? 0 : -1;
}

static void stm32_delay_ms(uint32_t ms, void *user_ctx)
{
    HAL_Delay(ms);
}

static lcd1602_i2c_t lcd;

void app_lcd_init(void)
{
	/* 1) Đảm bảo I2C đã init xong */
	    MX_I2C1_Init();

	    /* 2) Chờ LCD + PCF8574 ổn định nguồn */
	    HAL_Delay(50);

    lcd = (lcd1602_i2c_t){
        .i2c_addr  = 0x27,
        .i2c_write = stm32_i2c_write,
        .delay_ms  = stm32_delay_ms,
        .user_ctx  = &hi2c1,
        .backlight = 1
    };

    lcd1602_init(&lcd);
    /* 5) Bật đèn nền ngay lập tức */
       lcd1602_set_backlight(&lcd, 1);
       lcd1602_cmd(&lcd, 0x00);   // gửi 1 byte để áp dụng BL_BIT
}








status_dev BMP280_I2C_Read(uint8_t dev, uint8_t reg, uint8_t *buf, uint16_t len)
{
    if (HAL_I2C_Mem_Read(&hi2c1, dev, reg,
                         I2C_MEMADD_SIZE_8BIT,
                         buf, len, 100) == HAL_OK)
        return STATUS_OK;
    return STATUS_ERR;
}

status_dev BMP280_I2C_Write(uint8_t dev, uint8_t reg, uint8_t *buf, uint16_t len)
{
    if (HAL_I2C_Mem_Write(&hi2c1, dev, reg,
                          I2C_MEMADD_SIZE_8BIT,
                          buf, len, 100) == HAL_OK)
        return STATUS_OK;
    return STATUS_ERR;
}

void BMP280_InitObject(void)
{
    bmp.dev_addr = BMP280_ADDR_LOW;
    bmp.read_reg = BMP280_I2C_Read;
    bmp.write_reg = BMP280_I2C_Write;
    bmp.delay    = HAL_Delay;

    BMP280_Init(&bmp);
}
void app_lcd_task(void)
{ float t = BMP280_ReadTemperature(&bmp);
float p = BMP280_ReadPressure(&bmp);

char ts[16], ps[16];
   float_to_string(ts, t);
   float_to_string(ps, p);

lcd1602_goto_xy(&lcd, 0, 0);
lcd1602_puts(&lcd, "Temp: ");
lcd1602_goto_xy(&lcd, 0, 6);
lcd1602_puts(&lcd, ts);

lcd1602_goto_xy(&lcd, 1, 0);
lcd1602_puts(&lcd, "ap xuat: ");
lcd1602_goto_xy(&lcd, 1, 7);
lcd1602_puts(&lcd, ps);
   /* lcd1602_goto_xy(&lcd, 0, 0);
    lcd1602_puts(&lcd, "Vo oi");
    lcd1602_goto_xy(&lcd, 1, 0);
    lcd1602_puts(&lcd, "cho a choi game");
    */
}
