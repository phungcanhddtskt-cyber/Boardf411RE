/*
 * lcd1602_pcf8574.h
 *
 *  Created on: 26 thg 5, 2026
 *      Author: PhungCanh
 */

#ifndef INC_LCD1602_PCF8574_H_
#define INC_LCD1602_PCF8574_H_


#include <stdint.h>

#ifdef __cplusplus
extern "C" {
#endif

/*----------------------------------------------------------
 * Status
 *---------------------------------------------------------*/
typedef enum {
    LCD1602_OK = 0,
    LCD1602_ERR_PARAM,
    LCD1602_ERR_BUS
} lcd1602_status_t;

/*----------------------------------------------------------
 * Config / context
 *---------------------------------------------------------*/
typedef struct {
    uint8_t  i2c_addr;   /* 7-bit address of PCF8574 */

    int (*i2c_write)(uint8_t addr,
                     const uint8_t *buf,
                     uint16_t len,
                     void *user_ctx);

    void (*delay_ms)(uint32_t ms, void *user_ctx);

    void *user_ctx;      /* e.g. I2C handle, context pointer */

    uint8_t backlight;   /* 0: off, 1: on */
} lcd1602_i2c_t;

/*----------------------------------------------------------
 * Public API
 *---------------------------------------------------------*/

/* Initialize LCD (4-bit mode, 2 lines, etc.) */
lcd1602_status_t lcd1602_init(lcd1602_i2c_t *lcd);

/* Send command byte (RS = 0) */
lcd1602_status_t lcd1602_cmd(lcd1602_i2c_t *lcd, uint8_t cmd);

/* Send data byte (RS = 1) */
lcd1602_status_t lcd1602_data(lcd1602_i2c_t *lcd, uint8_t data);

/* Print C string (null-terminated) at current cursor */
lcd1602_status_t lcd1602_puts(lcd1602_i2c_t *lcd, const char *str);

/* Set cursor (row, col) – row: 0/1, col: 0..15 */
lcd1602_status_t lcd1602_goto_xy(lcd1602_i2c_t *lcd,
                                 uint8_t row,
                                 uint8_t col);

/* Clear display */
lcd1602_status_t lcd1602_clear(lcd1602_i2c_t *lcd);

/* Control backlight (0: off, 1: on) */
lcd1602_status_t lcd1602_set_backlight(lcd1602_i2c_t *lcd, uint8_t on);

#ifdef __cplusplus
}
#endif




#endif /* INC_LCD1602_PCF8574_H_ */
