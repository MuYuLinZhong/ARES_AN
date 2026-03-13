/* ============================================================================
** Copyright (c) 2023 Infineon Technologies AG
**               All rights reserved.
**               www.infineon.com
** ============================================================================
**
** ============================================================================
** Redistribution and use of this software only permitted to the extent
** expressly agreed with Infineon Technologies AG.
** ============================================================================
*
*/

/**
 * @file     ring_buffer.h
 *
 * @brief    Smack NVM application code file, helper file for UART library.
 *
 * @version  v1.0
 * @date     2023-04-18
 *
 * @note
 */


#ifndef _RING_BUFFER_
#define _RING_BUFFER_

#include <stdint.h>
#include <stdbool.h>
#include "cmsis_gcc.h"

/**
 * @brief type definition of ring buffer struct that is used for storing tx and rx buffers while using UART
 */
struct ring_buffer
{
    uint8_t* buffer;        ///< pointer to the ring buffer
    uint8_t size;           ///< size of the ring buffer
    volatile uint8_t head;  ///< points at the beginning of the ring buffer
    volatile uint8_t tail;  ///< points at the end of the ring buffer
};


/**
 * @brief   This function stores the data given in the nearest available space in the ring buffer given, the caller must check
 *          before calling this function if the buffer is full or not
 *
 * @param   rb a ring buffer to store data in
 * @data    data this gets stored in the ring buffer of typte uint8_t
 *
 */
extern void ring_buffer_put(struct ring_buffer* rb, uint8_t data);


/**
 * @brief   This function returns the oldest data in the ring buffer given, the caller must check
 *          before calling this function if the buffer is empty.
 *          Notice the pointer to the data read changes after reading to the next one: meaning whenever it reads once,
 *          the data is not readable anymore and considered deleted from the buffer
 *
 * @param   rb a ring buffer to read data from
 *
 * @return  oldest data in the ring buffer of typte uint8_t
 *
 */
__STATIC_FORCEINLINE uint8_t ring_buffer_get(struct ring_buffer* rb)
{
    const uint8_t data = rb->buffer[rb->tail];
    rb->tail++;

    if (rb->tail >= rb->size)
    {
        rb->tail = 0;
    }

    return data;
}


/**
 * @brief   This function returns the oldest data in the ring buffer given, the caller must check
 *          before calling this function if the buffer is empty.
 *          Notice the pointer to the data read doesn't change: meaning it only reads and doesn't delete anything from the buffer
 *
 * @param   rb a ring buffer to read data from
 *
 * @return  oldest data in the ring buffer of typte uint8_t
 *
 */
extern uint8_t ring_buffer_peek(const struct ring_buffer* rb);


/**
 * @brief   this function checks if the ring buffer is emty or not and should be called before using the get and peek functions
 *
 * @param   rb a ring buffer to check
 *
 * @return  This function returns true if the buffer given is empty and false if the buffer is not empty
 *
 */
__STATIC_FORCEINLINE bool ring_buffer_is_empty(const struct ring_buffer* rb)
{
    return rb->head == rb->tail;
}


__STATIC_FORCEINLINE uint8_t ring_buffer_available_characters(const struct ring_buffer* rb)
{
    return ((rb->tail - rb->head) + rb->size) % rb->size;
}

/**
 * @brief   this function checks if the ring buffer is full or not and should be called before using the put function
 *
 * @param   rb a ring buffer to check
 *
 * @return  This function returns true if the buffer given is full and false if the buffer is not full
 *
 */
extern bool ring_buffer_is_full(const struct ring_buffer* rb);


/**
 * @brief   this function empties the ring buffer given
 *
 * @param   rb a ring buffer to be emptied
 *
 */
__STATIC_FORCEINLINE void ring_buffer_clear(struct ring_buffer* rb)
{
    rb->head = rb->tail = 0;
}


#endif /* _RING_BUFFER_ */
