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
 * @file     i2c_lib.h
 *
 * @brief   library for using i2c
 *
 * @version  v1.1
 * @date     2023-18-07
 *
 * @note
 */


#ifndef _I2C_LIB_
#define _I2C_LIB_

#include <stdint.h>
#include <stdbool.h>


/**
 * @brief I2C address flag
 *
 * I2C addresses are passed right-justified, e.g., in 7-bit notation for the standard
 * addressing mode.
 * 10-bit addresses are marked with bit 15 (LSB of uint16_t) set to 1. Please note that
 * there is not support for 10-bit addressing in slave mode, and only limited support
 * in master mode.
 */
#define I2C_ADDRESS_10BIT       (0x8000)        //!< flag to indicate that I2C address is a 10-bit address


/**
 * @brief enumeration that differentiate between master and slave mode for i2c
 *
 */
typedef enum
{
    i2c_master = 0x1,
    i2c_slave = 0x2,
    i2c_master_slave = (i2c_master | i2c_slave)
} i2c_mode_t;

/**
 * @brief enumeration tells the reason why the callback function was called
 *
 */
typedef enum
{
    i2c_start_transfer = 0x1,
    i2c_continue_transfer = 0x2,
    i2c_transfer_finished = 0x4
} i2c_callback_reason_t;

/**
 * @brief configuration for master and slave
 *
 *  The communication speed in master mode is configured in m_speed. The I2C controller is able to
 *  handle communication at fast speed (400kHz) or slower.
 *  The slave speed in s_speed defines the sampling frequency of the I2C bus lines which in turn
 *  gives a maximum incoming bus speed that can be handled in slave mode. The I2C master may be
 *  communicate at a lower speed but not at a faster. Configure the slave speed to the lowest value
 *  suitable to commnicate with the I2C bus masters for better filtering of noise.
 *  There is a mutual relation between m_speed and s_speed. The slave speed must be configured to
 *  at least the value of m_speed, but can only be about 10 times of m_speed. If the speed configured
 *  for master mode im m_speed is to slow when compared to s_speed, the master mode speed will
 *  silently be increased to fulfil the s_speed requirements.
 *  If the device is only used in master mode on the I2C, configure the desired bus speed in m_speed,
 *  and set the same value in s_speed. If the device is only used as an I2C slave, configure the
 *  speed of the fastest master on the I2C bus in s_speed. If the device shall operate as a master
 *  and a slave on a bus with multiple master nodes, configure the speed of the fastest master in
 *  s_speed, and the desired speed for this device when communication as a master in m_speed.
 *
 *  GCE and slave address are ignored in Master mode
 */
typedef struct
{
    uint16_t m_speed;       //!< speed in master mode, given in kbit/s
    uint16_t s_speed;       //!< maximum expected incoming speed when operating in slave mode, given in kbit/s.
    i2c_mode_t mode;        //!< mode can be either master mode or slave mode
    uint8_t irq_num;        //!< sets interrupt request source of matrix, should be between 9 and 14 and must be consistent with the settings in the APARAM block (usually in the sl_aparam.c file)
    bool pullup;            //!< activate internal pullups (external pullups preferred)
    //what comes after is only relevant for slave mode or master_slave mode
    bool GCE;               //!< if true, it activates general call address in case of slave mode
    uint16_t slave_address; //!< sets slave address in case of slave mode communication. The address is given in 7-bit notation

} i2c_config_t;

/**
 * @brief Error codes returned by I2C functions
 *
 * All errors are reported as negative numbers.
 * Positive numbers may indicate number of successfully transfered bytes.
 */
typedef enum
{
    I2C_OK         = 0,    //!< success, no error
    I2C_ERROR      = -1,   //!< general error
    I2C_NACK_ADDR  = -2,   //!< device address not acknowledged -> no slave device at address?
    I2C_NACK       = -3,   //!< slave did not acknowledge
    I2C_TIMEOUT    = -4,   //!< timeout while waiting for slave (clock stretching)
    I2C_BUS_BUSY   = -5,   //!< bus is busy or stuck
    I2C_BUS_ERROR  = -6,   //!< bus error (stuck?)
    I2C_ARG        = -7,   //!< arguments out of range or configuration not set
} i2c_error_t;

/**
 * @brief  This function is to be called to configure all parameters of I2C communication
 * @param  config configuration parameters for I2C, explained above
 * @return error code
 */
extern i2c_error_t i2c_config(const i2c_config_t* config);

/**
 * @brief  This function is to be called after i2c config and before any master transfer, and also in case of slave mode to complete initialization fo slave
 *
 * @return error code
 */
extern i2c_error_t i2c_open(void);

/**
 * @brief  This function is to be called at the end of I2C communication to turn of I2C and reset settings
 *
 * @param dev_addr  I2C slave device address 7-bit notation
 * @param reg_addr  register address (optional)
 * @param reg_size  size of register address: 0 to 4 bytes (0 indicates no register address to be sent)
 * @param rx_buffer buffer to place received data in
 * @param rx_size   number of bytes to read from I2C slave device
 * @param tx_buffer buffer with data to be transmitted to I2C slave device
 * @param tx_size   number of bytes to be transmitted
 *
 * @return error code
 */
extern i2c_error_t i2c_transfer(const uint16_t dev_addr, const uint32_t reg_addr, const uint8_t reg_size, const void* tx_buffer, const uint16_t tx_size, void* rx_buffer, const uint16_t rx_size);

/**
 * @brief  This function is to be called at the end of I2C communication to turn of I2C and reset settings
 *
 * @return error code
 */
extern i2c_error_t i2c_close(void);

/**
 * @brief  This function is to be called after any master transfer to ensure communication is done
 * @param   timeout if timeout is set to zero, timeout will be deactivated (or set to infinity) (not recommended)
 *          else each timeout count is 33 ticks, e.g. 1ms = 848 timeout (apporx.) Be aware that the timeout here stands for the whole transaction
 *
 * @return error code of the last transfer
 */
extern i2c_error_t i2c_wait(uint32_t timeout);


/**
 * @brief  This function returns the previous transfer's successfully sent bytes as a master
 *
 * @return successfully amount of bytes sent in the previous transfer
 */
extern uint16_t i2c_master_get_num_of_transmitted_bytes(void);

/**
 * @brief  This function returns the previous transfer's successfully received bytes as a master
 *
 * @return successfully amount of bytes reeived in the previous transfer
 */
extern uint16_t i2c_master_get_num_of_received_bytes(void);

/**
 * @brief  This function returns the previous transfer status
 *
 * @return error code of the last transfer
 */
extern i2c_error_t i2c_get_transfer_status(void);

/**
 * @brief  This function resets the bus, and can be called in master mode after a returned I2C error status
 *
 */
void i2c_recovery_cycle(void);

/**
 * @brief  Some shorter, more convenient functions: write a raw buffer to an I2C slave device.
 *
 * @param dev_addr  I2C slave device address 7-bit notation
 * @param tx_buffer buffer with data to be transmitted to I2C slave device
 * @param tx_size   number of bytes to be transmitted
 * @return error code
 */
static inline i2c_error_t i2c_write_raw(const uint16_t dev_addr, const uint8_t* tx_buffer, const uint16_t tx_size)
{
    return i2c_transfer(dev_addr, 0, 0, tx_buffer, tx_size, 0, 0);
}

/**
 * @brief  Some shorter, more convenient functions: read raw data from an I2C slave device
 *
 * @param dev_addr  I2C slave device address 7-bit notation
 * @param rx_buffer buffer to place received data in
 * @param rx_size   number of bytes to read from I2C slave device
 * @return error code
 */
static inline i2c_error_t i2c_read_raw(const uint16_t dev_addr, uint8_t* rx_buffer, const uint16_t rx_size)
{
    return i2c_transfer(dev_addr, 0, 0, 0, 0, rx_buffer, rx_size);
}

/**
 * @brief  Some shorter, more convenient functions: first select a register or memory address, then write data to the I2C slave device.
 *
 * @param dev_addr  I2C slave device address 7-bit notation
 * @param reg_addr  register address (optional)
 * @param reg_size  size of register address: 0 to 4 bytes (0 indicates no register address to be sent)
 * @param tx_buffer buffer with data to be transmitted to I2C slave device
 * @param tx_size   number of bytes to be transmitted
 * @return error code
 */
static inline i2c_error_t i2c_write_reg(const uint16_t dev_addr, const uint32_t reg_addr, const uint8_t reg_size, const uint8_t* tx_buffer, const uint16_t tx_size)
{
    return i2c_transfer(dev_addr, reg_addr, reg_size, tx_buffer, tx_size, 0, 0);
}

/**
 * @brief  Some shorter, more convenient functions: first select a register or memory address, then read data from the I2C slave device.
 *
 * @param dev_addr  I2C slave device address 7-bit notation
 * @param reg_addr  register address (optional)
 * @param reg_size  size of register address: 0 to 4 bytes (0 indicates no register address to be sent)
 * @param rx_buffer buffer to place received data in
 * @param rx_size   number of bytes to read from I2C slave device
 * @return error code
 */
static inline i2c_error_t i2c_read_reg(const uint16_t dev_addr, const uint32_t reg_addr, const uint8_t reg_size, void* rx_buffer, const uint16_t rx_size)
{
    return i2c_transfer(dev_addr, reg_addr, reg_size, 0, 0, rx_buffer, rx_size);
}

/**
 * @brief  This is a callback function used to get the transmitting buffer and size from the user
 *
 * @param data          pointer to the address of the tx buffer
 * @param size          pointer to the size of the tx buffer
 * @param cb_receive_t  this determines the reason why the function is called
 *
 */
extern void i2c_slave_update_transmit_buffer(uint8_t** data, uint16_t* size, i2c_callback_reason_t cb_transmit_reason);

/**
 * @brief  This is a callback function used to get the receiving buffer and size from the user
 *
 * @param data          pointer to the address of the rx buffer
 * @param size          pointer to the size of the rx buffer
 * @param cb_receive_r  this determines the reason why the function is called
 *
 */
extern void i2c_slave_update_receive_buffer(uint8_t** data, uint16_t* size, i2c_callback_reason_t cb_receive_reason);


/**
 * @brief  This functions tells if transfer is complete in master mode
 *
 * @return  true if communication is completed
 */
extern bool i2c_master_is_transfer_complete(void);


/**
 * @brief  This functions tells if master sent data
 *
 * @return  true if data received during slave communication
 */
extern bool i2c_slave_is_data_received(void);

/**
 * @brief  This functions tells if master received data
 *
 * @return  true if data transmitted during slave communication
 */
extern bool i2c_slave_is_data_transmitted(void);

/**
 * @brief  This functions resets the receiving flag to false and resets last tx transition counter to zero
 */
extern void i2c_slave_reset_receive_flag(void);

/**
 * @brief  This functions resets the transmitting flag to false and resets last tx transition counter to zero
 */
extern void i2c_slave_reset_transmit_flag(void);

/**
 * @brief  This functions returns the total number of transmitted bytes since i2c is configured
 *
 * @return  number of successfully transmitted bytes since configuration
 */
extern uint16_t i2c_slave_get_total_num_of_transmitted_bytes(void);

/**
 * @brief  This functions returns the number of transmitted bytes since tx slave buffer was updated
 * The index is reseted automatically after returning from the callback function
 * In case of increasing the size of the buffer without changing the initial buffer address, the index is not reseted and continues transferring
 *
 * @return  number of successfully transmitted bytes since tx slave buffer updated
 */
extern uint16_t i2c_slave_get_last_transfer_tx_byte_count(void);

/**
 * @brief  This functions returns the total number of received bytes since i2c is configured
 *
 * @return  number of successfully received bytes since configuration
 */
extern uint16_t i2c_slave_get_total_num_of_received_bytes(void);

/**
 * @brief  This functions returns the number of received bytes since rx slave buffer was updated
 * The index is reseted automatically after returning from the callback function
 *In case of increasing the size of the buffer without changing the initial buffer address, the index is not reseted and continues transferring
 *
 * @return  number of successfully received bytes since rx slave buffer updated
 */
extern uint16_t i2c_slave_get_last_transfer_rx_byte_count(void);

/**
 * @brief  This functions enables or disables i2c bus for the chip
 * @param  on_off  if true, turns on the I2C bus, if false turns it off
 */
extern void i2c_bus_enable(bool on_off);


/**
 * @brief  interrupt service routine for handling bothe master and slave modes
 *
 */
extern void i2c_interrupt_service_routine(void);

#endif /* _I2C_LIB_ */
