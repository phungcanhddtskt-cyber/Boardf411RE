/*
 * lcd1602_pcf8574.c
 *
 *  Created on: 26 thg 5, 2026
 *      Author: PhungCanh
 */

#include "lcd1602_pcf8574.h"
#include "string.h"
/*----------------------------------------------------------
 * PCF8574 bit mapping (tùy module, chỉnh ở đây)
 *
 * P0 -> D4
 * P1 -> D5
 * P2 -> D6
 * P3 -> D7
 * P4 -> EN
 * P5 -> RW
 * P6 -> RS
 * P7 -> Backlight
 *---------------------------------------------------------*/
#define LCD_EN_BIT   (1U << 2)
#define LCD_RW_BIT   (1U << 1)
#define LCD_RS_BIT   (1U << 0)
#define LCD_BL_BIT   (1U << 3)

/*----------------------------------------------------------
 * Internal helpers
 *---------------------------------------------------------*/
static lcd1602_status_t lcd1602_write_nibble(lcd1602_i2c_t *lcd,
                                             uint8_t nibble,
                                             uint8_t rs);

static lcd1602_status_t lcd1602_write_byte(lcd1602_i2c_t *lcd,
                                           uint8_t byte,
                                           uint8_t rs);

/*----------------------------------------------------------
 * Public API
 *---------------------------------------------------------*/

lcd1602_status_t lcd1602_init(lcd1602_i2c_t *lcd)
{
    if (!lcd || !lcd->i2c_write || !lcd->delay_ms) {
        return LCD1602_ERR_PARAM;
    }

    /* Wait for LCD power-up */
    lcd->delay_ms(40, lcd->user_ctx);

    /* Init sequence (4-bit mode) – HD44780 standard */
    /* Send 0x3 three times (high nibble only) */
    for (int i = 0; i < 3; i++) {
        (void)lcd1602_write_nibble(lcd, 0x03, 0);
        lcd->delay_ms(5, lcd->user_ctx);
    }

    /* Set 4-bit mode: send 0x2 (high nibble only) */
    (void)lcd1602_write_nibble(lcd, 0x02, 0);
    lcd->delay_ms(5, lcd->user_ctx);

    /* Function set: 4-bit, 2 lines, 5x8 dots: 0x28 */
    (void)lcd1602_cmd(lcd, 0x28);

    /* Display off: 0x08 */
    (void)lcd1602_cmd(lcd, 0x08);

    /* Clear display: 0x01 */
    (void)lcd1602_cmd(lcd, 0x01);
    lcd->delay_ms(2, lcd->user_ctx);

    /* Entry mode: increment, no shift: 0x06 */
    (void)lcd1602_cmd(lcd, 0x06);

    /* Display on, cursor off, blink off: 0x0C */
    (void)lcd1602_cmd(lcd, 0x0C);

    return LCD1602_OK;
}

lcd1602_status_t lcd1602_cmd(lcd1602_i2c_t *lcd, uint8_t cmd)
{
    return lcd1602_write_byte(lcd, cmd, 0); /* RS = 0 */
}

lcd1602_status_t lcd1602_data(lcd1602_i2c_t *lcd, uint8_t data)
{
    return lcd1602_write_byte(lcd, data, 1); /* RS = 1 */
}

lcd1602_status_t lcd1602_puts(lcd1602_i2c_t *lcd, const char *str)
{
    if (!lcd || !str) {
        return LCD1602_ERR_PARAM;
    }

    while (*str) {
        lcd1602_status_t st = lcd1602_data(lcd, (uint8_t)*str++);
        if (st != LCD1602_OK) {
            return st;
        }
    }
    return LCD1602_OK;
}

lcd1602_status_t lcd1602_goto_xy(lcd1602_i2c_t *lcd,
                                 uint8_t row,
                                 uint8_t col)
{
    if (!lcd || row > 1 || col > 15) {
        return LCD1602_ERR_PARAM;
    }

    uint8_t addr = (row == 0) ? (0x00 + col) : (0x40 + col);
    return lcd1602_cmd(lcd, 0x80 | addr);
}

lcd1602_status_t lcd1602_clear(lcd1602_i2c_t *lcd)
{
    lcd1602_status_t st = lcd1602_cmd(lcd, 0x01);
    if (st != LCD1602_OK) {
        return st;
    }
    if (lcd && lcd->delay_ms) {
        lcd->delay_ms(2, lcd->user_ctx);
    }
    return LCD1602_OK;
}

lcd1602_status_t lcd1602_set_backlight(lcd1602_i2c_t *lcd, uint8_t on)
{
    if (!lcd) {
        return LCD1602_ERR_PARAM;
    }
    lcd->backlight = (on ? 1U : 0U);
    /* Không gửi gì ngay; backlight sẽ áp dụng ở lần ghi tiếp theo */
    return LCD1602_OK;
}

/*----------------------------------------------------------
 * Internal helpers
 *---------------------------------------------------------*/

static lcd1602_status_t lcd1602_write_nibble(lcd1602_i2c_t *lcd,
                                             uint8_t nibble,
                                             uint8_t rs)
{
    if (!lcd || !lcd->i2c_write) {
        return LCD1602_ERR_PARAM;
    }

    uint8_t data = 0;

    /* Put nibble on D4..D7 (P0..P3) */
    data |= (nibble <<4);

    /* RS: 0 = command, 1 = data */
    if (rs) {
        data |= LCD_RS_BIT;
    }

    /* RW = 0 (write only) – RW pin usually tied low, but keep bit clear */
    data &= (uint8_t)~LCD_RW_BIT;

    /* Backlight */
    if (lcd->backlight) {
        data |= LCD_BL_BIT;
    }

    uint8_t buf[2];

    /* EN = 1 */
    buf[0] = data | LCD_EN_BIT;
    /* EN = 0 */
    buf[1] = data & (uint8_t)~LCD_EN_BIT;

    if ((lcd->i2c_write)(lcd->i2c_addr, buf, 2, lcd->user_ctx) != 0) {
        return LCD1602_ERR_BUS;
    }

    if (lcd->delay_ms) {
        lcd->delay_ms(1, lcd->user_ctx);  // lcd loadinggg
    }

    return LCD1602_OK;
}

static lcd1602_status_t lcd1602_write_byte(lcd1602_i2c_t *lcd,
                                           uint8_t byte,
                                           uint8_t rs)
{
    if (!lcd) {
        return LCD1602_ERR_PARAM;
    }

    uint8_t high = (uint8_t)((byte >> 4) & 0x0F);
    uint8_t low  = (uint8_t)(byte & 0x0F);

    lcd1602_status_t st;

    /* High nibble first */
    st = lcd1602_write_nibble(lcd, high, rs);
    if (st != LCD1602_OK) {
        return st;
    }

    /* Then low nibble */
    st = lcd1602_write_nibble(lcd, low, rs);
    if (st != LCD1602_OK) {
        return st;
    }

    return LCD1602_OK;
}
