/*
 * ringBuffer.c
 *
 * Created: 6/19/2013 11:22:43 AM
 *  Author: jcmcclu2
 */ 

#include "ringBuffer.h"

void ringBuffer_initialize(RingBuffer* buff, uint8_t* buffer_array, uint8_t sz){
	// Clear low order bits until the size is a power of two.
	uint8_t i = 0;
	while(sz & (sz - 1)){
		sz &= ~(1<<i);
		++i;
	}
	
	buff->buffer = buffer_array;
	buff->size = sz;
	buff->mask = sz - 1;
	buff->head = 0;
	buff->tail = 0;
}

bool ringBuffer_canRead(RingBuffer* buff){
	return !(buff->tail == buff->head);
}

uint8_t ringBuffer_read(RingBuffer* buff){
	unsigned char tmptail;
	
	if(buff->head == buff->tail){
		return 0;
	}
	
	tmptail = (buff->tail + 1) & buff->mask;
	buff->tail = tmptail;
	return buff->buffer[tmptail];
}

uint8_t ringBuffer_read_blocking(RingBuffer* buff){
	unsigned char tmptail;
	
	while(buff->tail == buff->head){
		// Wait until the buffer is ready.
	}
	
	tmptail = (buff->tail + 1) & buff->mask;
	buff->tail = tmptail;
	return buff->buffer[tmptail];
}

bool ringBuffer_canWrite(RingBuffer* buff){
	return !((( buff->head + 1) & buff->mask) == buff->tail);
}

void ringBuffer_write_blocking(RingBuffer* buff, uint8_t data){
	uint8_t tmphead = ( buff->head + 1) & buff->mask;
	
	while(tmphead == buff->tail){
		// Wait until there's space in the buffer.
	}
	
	buff->head = tmphead;
	buff->buffer[tmphead] = data;
}

void ringBuffer_write(RingBuffer* buff, uint8_t data){
	uint8_t tmphead = ( buff->head + 1) & buff->mask;
	
	if(tmphead == buff->tail){
		// Overwrite the old contents of the buffer.
		buff->tail = (tmphead + 1) & buff->mask;
	}
	
	buff->head = tmphead;
	buff->buffer[tmphead] = data;
}