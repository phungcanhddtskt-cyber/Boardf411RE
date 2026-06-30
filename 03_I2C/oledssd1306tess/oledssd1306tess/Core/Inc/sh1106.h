/*
 * sh1106.h
 *
 *  Created on: 16 thg 5, 2026
 *      Author: PhungCanh
 */

#ifndef INC_SH1106_H_
#define INC_SH1106_H_

#include "stm32f4xx_hal.h"
#include <stdint.h>

#define SH1106_WIDTH    128
#define SH1106_HEIGHT   64
#define SH1106_I2C_ADDR (0x3C << 1)

void SH1106_Init(void);
void SH1106_UpdateScreen(void);
void SH1106_Fill(uint8_t color);
void SH1106_DrawPixel(uint8_t x, uint8_t y, uint8_t color);

void SH1106_SetCursor(uint8_t x, uint8_t y);
void SH1106_PutChar(char c);
void SH1106_Puts(char *str);


#endif /* INC_SH1106_H_ */
