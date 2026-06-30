/*
 * ringbuffer.h
 *
 *  Created on: 18 thg 4, 2026
 *      Author: PhungCanh
 */

#ifndef INC_RINGBUFFER_H_
#define INC_RINGBUFFER_H_

#define RB_SIZE 128
#include <stdint.h>
typedef struct {
	uint8_t buf[RB_SIZE];
	volatile uint16_t head;
	volatile uint16_t tail;
} RingBuffer;

void RB_Init(RingBuffer *rb);
uint8_t RB_Push(RingBuffer *rb, uint8_t data);
uint8_t RB_Pop(RingBuffer *rb, uint8_t *data);

#endif /* INC_RINGBUFFER_H_ */
