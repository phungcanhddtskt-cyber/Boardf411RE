/*
 * fl_to_str.h
 *
 *  Created on: 3 thg 6, 2026
 *      Author: PhungCanh
 */

#ifndef INC_FL_TO_STR_H_
#define INC_FL_TO_STR_H_

#include <stdint.h>

void float_to_string(char *buf, float value);
void float_to_string_prec(char *buf, float value, uint8_t prec);


#endif /* INC_FL_TO_STR_H_ */
