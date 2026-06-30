/*
 * ringbuffer.c
 *
 *  Created on: 18 thg 4, 2026
 *      Author: PhungCanh
 */


#include "ringbuffer.h"
void RB_Init(RingBuffer *rb)
{
	rb->head=0;
	rb->tail=0;
}
uint8_t RB_Push(RingBuffer *rb, uint8_t data)
{
	uint8_t next = (rb->head+1)% RB_SIZE; // (rb->head+1)&(RB_SIZE-1) buf = 2^n moi dung dc
	if(next == rb->tail) // full data chay het 1 vong buffer roi
		return 0;
	rb->buf[rb->head]=data;
	rb->head=next;
	return 1;
}
uint8_t RB_Pop(RingBuffer *rb, uint8_t *data)
{
	if(rb->head == rb->tail) // empty data
		return 0;
	*data = (rb->buf[rb->tail]);
	rb->tail = (rb->tail+1)% RB_SIZE; // (rb->tail+1)&(RB_SIZE-1)
	return 1;
}
