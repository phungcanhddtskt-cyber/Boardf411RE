/*
 * sh1106.c
 *
 *  Created on: 16 thg 5, 2026
 *      Author: PhungCanh
 */


#include "sh1106.h"
#include <stdlib.h>

extern I2C_HandleTypeDef hi2c1;

static uint8_t buffer[SH1106_WIDTH * SH1106_HEIGHT / 8];
static uint8_t cursor_x = 0;
static uint8_t cursor_y = 0;

// ================= FONT 6x8 =================
static const uint8_t font6x8[][6] = {
    {0x00,0x00,0x00,0x00,0x00,0x00},
    {0x00,0x00,0x5F,0x00,0x00,0x00},

};

// ================= LOW LEVEL =================
static void sh1106_WriteCmd(uint8_t cmd)
{
    uint8_t data[2] = {0x00, cmd};
    HAL_I2C_Master_Transmit(&hi2c1, SH1106_I2C_ADDR, data, 2, 100);
}

static void sh1106_WriteData(uint8_t *data, uint16_t size)
{
    uint8_t control = 0x40;
    HAL_I2C_Master_Transmit(&hi2c1, SH1106_I2C_ADDR, &control, 1, 100);
    HAL_I2C_Master_Transmit(&hi2c1, SH1106_I2C_ADDR, data, size, 100);
}

// ================= INIT =================
void SH1106_Init(void)
{
    HAL_Delay(100);

    sh1106_WriteCmd(0xAE);        // Display OFF
    sh1106_WriteCmd(0xD5); sh1106_WriteCmd(0x80);
    sh1106_WriteCmd(0xA8); sh1106_WriteCmd(0x3F);
    sh1106_WriteCmd(0xD3); sh1106_WriteCmd(0x00);
    sh1106_WriteCmd(0x40);
    sh1106_WriteCmd(0xA1);
    sh1106_WriteCmd(0xC8);
    sh1106_WriteCmd(0xDA); sh1106_WriteCmd(0x12);

    // ⭐ MODULE GME12864-11-12-13 DÙNG EXTERNAL VCC
    sh1106_WriteCmd(0xAD);
    sh1106_WriteCmd(0x8A);   // external VCC (KHÔNG dùng 0x8B)

    sh1106_WriteCmd(0x81); sh1106_WriteCmd(0x7F);
    sh1106_WriteCmd(0xD9); sh1106_WriteCmd(0x22);
    sh1106_WriteCmd(0xDB); sh1106_WriteCmd(0x40);
    sh1106_WriteCmd(0xA4);
    sh1106_WriteCmd(0xA6);
    sh1106_WriteCmd(0xAF);   // Display ON

    SH1106_Fill(0);
    SH1106_UpdateScreen();
}

// ================= CORE =================
void SH1106_Fill(uint8_t color)
{
    for (uint32_t i = 0; i < sizeof(buffer); i++)
        buffer[i] = color ? 0xFF : 0x00;
}

void SH1106_DrawPixel(uint8_t x, uint8_t y, uint8_t color)
{
    if (x >= SH1106_WIDTH || y >= SH1106_HEIGHT) return;

    uint16_t index = x + (y / 8) * SH1106_WIDTH;
    uint8_t bit = 1 << (y % 8);

    if (color) buffer[index] |= bit;
    else       buffer[index] &= ~bit;
}

void SH1106_UpdateScreen(void)
{
    uint8_t padding[4] = {0,0,0,0};

    for (uint8_t page = 0; page < 8; page++)
    {
        sh1106_WriteCmd(0xB0 + page);
        sh1106_WriteCmd(0x02);   // offset
        sh1106_WriteCmd(0x10);

        sh1106_WriteData(&buffer[SH1106_WIDTH * page], SH1106_WIDTH);
        sh1106_WriteData(padding, 4);   // gửi đủ 132 byte/page
    }
}

// ================= TEXT =================
void SH1106_SetCursor(uint8_t x, uint8_t y)
{
    cursor_x = x;
    cursor_y = y;
}

void SH1106_PutChar(char c)
{
    if (c < 32 || c > 126) return;

    const uint8_t *ch = font6x8[c - 32];

    for (int i = 0; i < 6; i++)
        for (int j = 0; j < 8; j++)
            SH1106_DrawPixel(cursor_x + i, cursor_y + j, (ch[i] >> j) & 1);

    cursor_x += 6;
}

void SH1106_Puts(char *str)
{
    while (*str)
        SH1106_PutChar(*str++);
}
