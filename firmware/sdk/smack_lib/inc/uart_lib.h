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
 * @file     uart_lib.h
 *
 * @brief   library for using uart
 *
 * @version  v1.0
 * @date     2023-04-18
 *
 * @note
 */


#ifndef _UART_LIB_
#define _UART_LIB_

#include <stdint.h>
#include <stdbool.h>
#include "uart_drv.h"


/**
 * @brief type definition to configure the UART module and the used UART protocol.
 */
typedef struct uart_line_control_s
{

    bool parity_en;         ///< if true then receive parity check is enabled and parity bit is transmitted
    parity_t even_odd;      ///< can be set to even or odd, only relevant, if parity_en==true
    bool two_stop_bits;     ///< if true, then two STOP bits are used, otherwise one STOP bit
    bool fifo_en;           ///< if true, then RX/TX Fifos are enabled
    uart_data_len_t word_length; ///< can be 5, 6, 7 or 8-bits. Please use the defined constants five, six, seven or eight
    bool  stick_parity;     ///< if true, then either constant '0' or '1' is sent as parity bit
} uart_line_control_t;

/**
 * @brief type definition to configure GPIOs used for UART transmit and receive
 */
typedef enum uart_gpio_e
{
    uart_gpio0_rx_gpio2_tx,
    uart_gpio2_rx_gpio0_tx
} uart_gpio_t;


/**
 * @brief   type definition for enabling or disabling UART, UART transmit and UART receive
 */
typedef struct uart_settings
{
    bool uart;  ///< this enables/disables UART communication
    bool rx;    ///< this enables/disables only rx, ONLY if true AND uart is also true, rx of UART will be on
    bool tx;    ///< this enables/disables only tx, ONLY if true AND uart is also true, tx of UART will be on
    uart_gpio_t uart_gpios; ///< this is to choose which of gpio0 and gpio2 will be rx and tx
} uart_settings_t;


/**
 * @brief   type definition for enabling different UART interrupt types
 */
typedef struct uart_interrupt_mask
{
    bool rx;
    bool tx;
    bool rx_timeout;
    bool framing_error;
    bool parity_error;
    bool break_error;
    bool overrun_error;

} uart_interrupt_mask_t;

/**
 * @brief   type definition for changing the level of fifo when interrupt occurs
 *          Level recommended RX Depth 1/4 TX Depth 1/8
 */
typedef enum interrupt_fifo_level
{
    uart_one_eighth         = 0x0,
    uart_one_quarter        = 0x1,
    uart_one_half           = 0x2,
    uart_three_quarter      = 0x3,
    uart_seven_eighth       = 0x4

} interrupt_fifo_level_t;

enum uart_error_mask_e
{
    UART_ERROR_OVERRUN_MASK = (1U << 10),       //!< Overrun error
    UART_ERROR_BREAK_MASK   = (1U << 9),        //!< Break error
    UART_ERROR_PARITY_MASK  = (1U << 8),        //!< Parity error
    UART_ERROR_FRAMING_MASK = (1U << 7),        //!< Framing error
};

enum uart_flag_mask_e
{
    UART_TXFE_MASK = (1U << 7),         //!< Transmit FIFO empty
    UART_RXFF_MASK = (1U << 6),         //!< Receive FIFO full
    UART_TXFF_MASK = (1U << 5),         //!< Transmit FIFO full
    UART_RXFE_MASK = (1U << 4),         //!< Receive FIFO empty
    UART_BUSY_MASK = (1U << 3),         //!< UART busy. This bit is set when a byte is written to the Tx FIFO and remains set until all bits, including stop bits, have been sent from the shift register.
};

/**
 * @brief This function configures the UART module and the used UART protocol, enables/disables UART, UART tx, and UART rx, and sets the UART baudrate
 *
 * @param uart_lc to configure the UART module and the used UART protocol.
 * @param uart_s  for enabling different UART interrupt types
 * @param baudrate UART baudrate
 *
 */
extern void uart_configure(uart_line_control_t uart_lc, uart_settings_t uart_s, uint32_t baudrate);

/**
 * @brief This function configures the UART IRQs.
 *
 * @param irq_num of UART (must be 9...14)
 * @param im interrupt mask for enabling different UART interrupts
 * @param fifo_level_receive  Fifo IRQ threshold level
 * @param fifo_level_transmit FIFO IRQ threshold level
 *
 */
extern void uart_interrupt_configure(const uint8_t irq_num, const uart_interrupt_mask_t im, const interrupt_fifo_level_t fifo_level_receive, const interrupt_fifo_level_t fifo_level_transmit);

/**
 * @brief This function disables all possible UART interrupts and clears pending flags from already initialized UART interrupts
 *
 */
extern void uart_interrupt_disable(void);

/**
 * @brief This function disables UART rx and tx
 *
 */
extern void uart_disable(void);

/**
 * @brief This function enables UART clock
 *
 */
extern void uart_clock_enable(void);

/**
 * @brief This function disables UART clock
 *
 */
extern void uart_clock_disable(void);

/**
 * @brief This function can be used to make the below settings, please don't use, in case customized setting is wished
 *
 *  uart_clock_enable();
 *  uart_interrupt_mask_t im = {true, true, true, true, true, true, true};
 *  interrupt_fifo_level_t fifo_level_r = uart_one_quarter;
 *  interrupt_fifo_level_t fifo_level_t = uart_one_eighth;
 *  uart_interrupt_configure(irq_num, im, fifo_level_r, fifo_level_t);
 *  uart_line_control_t uart_lc = {false, even, false, true, uart_bits_8, false};
 *  uart_settings_t uart_s = {true, true, true, uart_gpio0_rx_gpio2_tx};
 *  uint32_t baudrate = 115200;
 *  uart_configure(uart_lc, uart_s, baudrate);
 *  number_of_erros = 0;
 *
 *  @param irq_num of UART (must be 9...14)
 *
 */
extern void uart_default_init(uint8_t irq_num);

/**
 * @brief this function sends the character c by UART
 *
 * @param c is the caharacter to be sent via UART of type uint8_t
 */
extern void uart_putchar(uint8_t c);

/**
 * @brief this function returns the character received by UART
 *
 * @return c is the character to get via UART of type uint8_t
 */
extern uint8_t uart_getchar(void);

/**
 * @brief  Send buffer through UART
 *
 * @param  buffer  pointer to the data that shall be sent
 * @param  size    number of bytes to send
 */
extern void uart_write(const void* buffer, const uint16_t size);


/**
 * @brief  Receive block of data through UART (non-blocking)
 *
 * @param  buffer  pointer to the receive buffer
 * @param  size    size of receive buffer in bytes
 *
 * @return number of bytes actually put into receive buffer
 */
extern uint16_t uart_read_if_available(void* buffer, const uint16_t size);

/**
 * @brief  Receive block of data through UART (blocking)
 *
 * @param  buffer  pointer to the receive buffer
 * @param  size    size of receive buffer in bytes
 *
 * @return number of bytes actually put into receive buffer
 */
extern uint16_t uart_read(void* buffer, const uint16_t size);

/**
 * @brief  this function returns if the there is an available characters in UART to read
 *
 * @return returns true if the there is an available characters in UART to read
 */
extern bool uart_read_is_available(void);

/**
 * @brief  this function returns the amount of available characters
 *
 * @return the amount of available characters
 */
extern uint8_t uart_read_available_characters(void);

/**
 * @brief   this function returns the amount of errors occurred during the UART communication
 *
 * @return  number of errors that has occurred during UART communication
 */
extern uint32_t uart_get_errors(void);

/**
 * @brief   This function returns the accumulated error flags of the UART since the last call of this function.
 *          The error flags are cleared when this function returns.
 *
 * @return  Error flags as a combination of the bit masks defined in uart_error_mask_e. Other bits may be set as well.

 */
extern uint16_t uart_get_error_flags(void);

/**
 * @brief   This function returns the UART flags.
 *
 *
 * @return  UART flags as a combination of the bit masks defined in uart_flag_mask_e. Other bits may be set as well.
 */
extern uint32_t uart_get_status_flags(void);

/**
 * @brief   This function waits until all data from the FIFO buffers has been transmitted by the UART.
 *
 * @param   timeout  number of loops to wait until the UART is idle.
 * @return  0: UART Tx operation completed; <>0: timeout reached
 */
extern uint32_t uart_flush_tx(const uint32_t timeout);


/**
 * @brief UART IRQ service request function. It serves rx, tx and other UART interrupt types
 *
 */
extern void uart_interrupt_service_routine(void);


#endif /* _UART_LIB_ */
