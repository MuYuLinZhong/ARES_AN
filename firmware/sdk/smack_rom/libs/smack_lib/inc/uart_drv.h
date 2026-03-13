/* ============================================================================
** Copyright (c) 2021 Infineon Technologies AG
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
 * @file     uart_drv.h
 *
 * @brief    Driver for the UART communication.
 *
 * @version  v1.0
 * @date     2020-05-20
 *
 * @note
 */

/*lint -save -e960 */

#ifndef _UART_DRV_H_
#define _UART_DRV_H_

#include <stdbool.h>

/** @addtogroup Infineon
 * @{
 */

/** @addtogroup Smack
 * @{
 */


/** @addtogroup uart
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif


typedef enum uart_data_len_e
{
    uart_bits_8 = 0x03, //!< eight UART date Bits
    uart_bits_7 = 0x02, //!< seven UART date Bits
    uart_bits_6 = 0x01, //!< six UART date Bits
    uart_bits_5 = 0x00, //!< five UART date Bits
} uart_data_len_t;

//#define nvm_erase            0x10
//#define nvm_program          0x11
//#define nvm_open_assi_buffer 0x12
//#define nvm_switch_on        0x13
//#define nvm_switch_off       0x14
//#define nvm_normal_read      0x15

/**
 * @brief type definition, to configure parity check scheme of UART protocoll
 */
typedef enum parity_e
{
    even,//!< even
    odd  //!< odd
} parity_t;
//

/**
 * @brief This function configure the UART Baudrate in the UART module. It considers the internal oscillator frequency
 * @param baudrate unit32_t to specify the baudrate (e.g. 9600, 19200 etc.)
 */
extern void     set_uart_baudrate(uint32_t baudrate); //e.g. set_uart_baudrate(9600);

/**
 * @brief This function configures the UART module and the used UART protocol.
 * @param parity_en bool, if true then receive parity check is enabled and parity bit is transmitted
 * @param even_odd  can be set to even or odd, only relevant, if parity_en==true
 * @param two_stop_bits if true, then two STOP bits are used, otherwise one STOP bit
 * @param fifo_en if true, then RX/TX Fifos are enabled
 * @param word_length can be 5, 6, 7 or 8-bits. Please use the defined constants five, six, seven or eight, which provide the expected code of the target HW
 * @param stick_parity if true, then either constant '0' or '1' is sent as parity bit
 *         PEN EPS SPS Parity bit (transmitted or checked)
 *         0   x   x   Not transmitted or checked
 *         1   1   0   Even parity
 *         1   0   0   Odd parity
 *         1   0   1   1
 *         1   1   1   0
 */
extern void     init_uart(bool parity_en, parity_t even_odd, bool two_stop_bits, bool fifo_en, uart_data_len_t word_length, bool  stick_parity);


/**
 * @brief This function controls the UART operation mode
 * @param enable if true, then UART module is enabled
 * @param rx_enable if true and enable== true, then RX is enabled
 * @param tx_enable if true and enable== true, then TX is enabled
 */
extern void     set_uart_control(bool enable, bool rx_enable, bool tx_enable);

/**
 * @brief This function transmits the bytes stored in tx_buffer.
 */
extern void     transmit_bytes(void);

/**
 * @brief UART IRQ service request function. It serves either a routine from NVM or serves the ROM dandel UART protocol.
 * @param number
 */
extern void     serve_uart_irq(uint8_t number);

/**
 * @brief This function configures the UART IRQs.
 * @param irq_num of UART (must be 9...14)
 * @param rx_fifo_level Fifo IRQ threshold level (should be 0...4) 0->1/8 full, 1->1/4 full , 2-> 1/2 full , 3-> 3/4 full , 4-> 7/8 full of 32 Entries
 * @param tx_fifo_level FIFO IRQ threshold level (should be 0...4) 0->1/8 full, 1->1/4 full , 2-> 1/2 full , 3-> 3/4 full , 4-> 7/8 full of 32 Entries
 * @param rx_int if true, then RX interrupt is enabled
 * @param tx_int if true, then TX interrupt is enabled
 */
extern void     configure_uart_irq(uint8_t irq_num, uint8_t rx_fifo_level, uint8_t tx_fifo_level, bool rx_int, bool tx_int);

/**
 * @brief this function concatenates 2 subsequent uint16_t of an uint16_t array to a 32-bit uint32_t
 * @param data array of 16-bit integers
 * @param offset index of first element (e.g. if 1, then data[1] and data[2] are concatenated
 * @return
 */
//extern uint32_t make1_of4(const uint16_t* data, uint8_t offset);

#ifdef __cplusplus
}
#endif

/** @} */ /* End of group uart block */


/** @} */ /* End of group Smack */

/** @} */ /* End of group Infineon */

#endif /* _UART_DRV_H_ */
