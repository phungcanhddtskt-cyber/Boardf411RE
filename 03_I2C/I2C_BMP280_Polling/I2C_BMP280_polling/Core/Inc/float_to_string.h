/*
 * float_to_string.h
 *
 *  Created on: 4 thg 5, 2026
 *      Author: PhungCanh
 */

#ifndef INC_FLOAT_TO_STRING_H_
#define INC_FLOAT_TO_STRING_H_
#include <stdint.h>

void float_to_string(char *buf, float value);
void float_to_string_prec(char *buf, float value, uint8_t prec);


#endif /* INC_FLOAT_TO_STRING_H_ */
