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
 * @file     smack_exchange.h
 *
 * @brief    This module provides a method to exchange data points (variables) with an NFC reader
 *
 *
 * @version  v1.0
 * @date     2021-08-01
 *
 */

#ifndef INC_SMACK_EXCHANGE_H_
#define INC_SMACK_EXCHANGE_H_

/** @addtogroup Infineon
 * @{
 */

/** @addtogroup Smack
 * @{
 */


/** @addtogroup smack_exchange
 * @{
 */

#include "aes_lib.h"


#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Data type of the data item to be read or written
 */
enum data_point_type_e
{
    data_point_bool   = 0x01,   //!< data point is bool
    data_point_int8   = 0x02,   //!< data point is int8
    data_point_uint8  = 0x03,   //!< data point is uint8
    data_point_int16  = 0x04,   //!< data point is int16
    data_point_uint16 = 0x05,   //!< data point is uint16
    data_point_int32  = 0x06,   //!< data point is int32
    data_point_uint32 = 0x07,   //!< data point is uint32
    data_point_int64  = 0x08,   //!< data point is int64
    data_point_uint64 = 0x09,   //!< data point is uint64
    data_point_array  = 0x10,   //!< data point is array (byte array of known, fixed length)
    data_point_string = 0x11,   //!< data point is string (array of characters with variable, size limited length)
    data_point_encrypt = 0x40,   //!< data exchange must be encrypted
    data_point_write  = 0x80    //!< mark data point as writeable
};

/**
 * @brief Mask to extract data type (and discard write flag) from type field
 */
#define DATA_POINT_TYPE_MASK    ((uint8_t)~(data_point_write | data_point_encrypt))

/**
 * @brief Entry in the list of data points to be exchanged
 *
 * All data points that shall be exchanged are defined in an array of type
 * data_point_entry_t. The items in this array must be sorted in ascending
 * order of "data_point_id".
 * This array is passed to smack_exchange_init(), together with the number
 * of elements in the list. When smack_exchange_handler() is configured
 * properly in APARAM, the NFC reader can read and write values without
 * the need to provide further functions in the firmware.
 *
 * The notify function, if configured, is called whenever the data point
 * of the respective entry was read or written. The data point ID is given
 * as the argument to the notification function, further information must
 * retrieved from the mailbox. The notification function is called from
 * interrupt context, keep it as short as possible.
 */
typedef struct data_point_entry_e
{
    uint16_t  data_point_id;      //!< unique ID
    uint8_t  data_type;           //!< data type, must match type in message
    uint8_t  length;              //!< storage length for array types
    void*    value;               //!< pointer to variable in RAM
    void     (*notify_rx)(uint16_t); //!< if set, function is called on msg rx after rx data was stored with arg data_point_id
    void     (*notify_tx)(uint16_t); //!< if set, function is called on msg tx before tx message is assembled with arg data_point_id
} data_point_entry_t;


/**
 * @brief  Initialize Smack data exchange with the data point table
 *
 *         In case encryption may be used to exchange data points between NFC reader and tag, the encryption key must be loaded
 *         prior to communication using the function smack_exchange_key_set().
 *
 * @param  data_point_table List with data points to be exchanged with the NFC reader. The caller must not change
 *                          this list during runtime (e.g. list is not copied by this function).
 * @param  count            Number of elements in list
 */
extern void smack_exchange_init(const data_point_entry_t* const data_point_table, const uint16_t count);

/**
 * @brief  Set a key to be used to encrypt and decrypt mailbox contents.
 *
 *         The key is stored by the data exchange library and can be restored by smack_exchange_key_restore() if the AES unit
 *         was loaded with another key for an encryption independent from data point exchange.
 */
extern void smack_exchange_key_set(const aes_block_t* key);

/**
 * @brief  Restore the current key used for data point exchange if AES unit was in use for other purpose.
 *
 *         When encrypting data with other keys using the AES unit, ensure that this operation and the data point exchange library
 *         do not interfere, e.g. encapsulate the encryption call into a critical section and restore the data exchange key before
 *         leaving this critical section. the critical section must be as short as possible to avoid data loss on the active
 *         communication interfaces such as NFC.
 */
extern void smack_exchange_key_restore(void);

/**
 * @brief  Data exchange handler function to be called by Smack protocol handler.
 *         This function must be listed in the appropriate APARAM member as one of the 16 user functions in NVM.
 *         It handles the exchange of the data points and reads and writes directly from or to the location where
 *         the data is stored. After a write, a user function is called if configured in order to notify the user
 *         firmware of an update.
 */
extern void smack_exchange_handler(void);

/**
 * @brief  Set alert flag.
 *         In the next response to an NFC reader's request the alert flag is set. The reader then may read user defined
 *         data points to find out the reason for this alert. The flag is reset once the NFC reader has read the status
 *         word defined in the data exchange protocol.
 */
extern void smack_exchange_alert(void);


#ifdef __cplusplus
}
#endif

/** @} */ /* End of group smack_exchange */

/** @} */ /* End of group Smack */

/** @} */ /* End of group Infineon */

#endif /* INC_SMACK_EXCHANGE_H_ */
