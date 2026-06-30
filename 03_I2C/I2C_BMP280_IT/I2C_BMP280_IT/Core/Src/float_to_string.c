/*
 * float_to_string.c
 *
 *  Created on: 6 thg 5, 2026
 *      Author: PhungCanh
 */

#include "float_to_string.h"
void float_to_string(char *buf, float value)
{
    int32_t v = (int32_t)(value * 100);   // scale x100
    int32_t ip = v / 100;                 // phần nguyên
    int32_t fp = v % 100;                 // phần thập phân

    if (fp < 0) fp = -fp;

    sprintf(buf, "%ld.%02ld", ip, fp);
}
void float_to_string_prec(char *buf, float value, uint8_t prec)
{
    int32_t scale = 1;
    for (uint8_t i = 0; i < prec; i++)
        scale *= 10;

    int32_t v = (int32_t)(value * scale);
    int32_t ip = v / scale;
    int32_t fp = v % scale;

    if (fp < 0) fp = -fp;

    sprintf(buf, "%ld.%0*ld", ip, prec, fp);
}
