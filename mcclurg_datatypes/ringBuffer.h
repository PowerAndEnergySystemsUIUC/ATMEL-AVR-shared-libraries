/*
 * ringBufferType.h
 *
 * Created: 6/7/2013 9:21:02 PM
 *  Author: jcmcclu2
 */ 


#ifndef RINGBUFFERTYPE_H_
#define RINGBUFFERTYPE_H_

#include <stdint.h>
#include "bool.h"

typedef struct{
	unsigned char* buffer;
	unsigned char  size;
	unsigned char  head;
	unsigned char  tail;
	unsigned char  mask;
} RingBuffer;

/************************************************************************/
/* Initializes the specified buffer. Size rounded down to closest power */
/* of two. Blocks indefinitely if size is zero. Note that the usable    */
/* space is one less than the rounded size. Because one byte is used    */
/* internally to keep track of the current buffer length.               */
/************************************************************************/
void ringBuffer_initialize(RingBuffer* buff, uint8_t* buffer_array, uint8_t sz);


/************************************************************************/
/* Tells whether the buffer can be read without blocking.               */
/************************************************************************/
bool ringBuffer_canRead(RingBuffer* buff);

/************************************************************************/
/* Reads from the buffer. If there's nothing in the buffer, returns 0.  */
/************************************************************************/
uint8_t ringBuffer_read(RingBuffer* buff);

/************************************************************************/
/* Block until there is something in the buffer, then read from it.     */
/************************************************************************/
uint8_t ringBuffer_read_blocking(RingBuffer* buff);


/************************************************************************/
/* Tells whether the buffer can be written to without overflow.         */
/************************************************************************/
bool ringBuffer_canWrite(RingBuffer* buff);

/************************************************************************/
/* Write to the buffer. Overwrites old data on overflow.                */
/************************************************************************/
void ringBuffer_write(RingBuffer* buff, uint8_t data);

/************************************************************************/
/* Block until the buffer has space, then write to it.                  */
/************************************************************************/
void ringBuffer_write_blocking(RingBuffer* buff, uint8_t data);

#endif /* RINGBUFFERTYPE_H_ */