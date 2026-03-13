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
 * @file     dand_handler.h
 *
 * @brief    Dand_Handler.
 *
 * @version  v1.0
 * @date     2020-06-06
 *
 * @note
 */

#ifndef _DAND_HANDLER_H_
#define _DAND_HANDLER_H_


/** @addtogroup Infineon
 * @{
 */

/** @addtogroup Smack
 * @{
 */


/** @addtogroup dand_handler
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

/*----------------------------------------------------------------------------
*      Constants
 *---------------------------------------------------------------------------*/


#define RX_BUFFER_SIZE 48u //!< @brief size of the receiving buffer in bytes.
#define TX_BUFFER_SIZE 48u //!< @brief size of the transmitting buffer in bytes.
#define MAILBOX_SIZE 64u //!< @brief size of the mailbox in words.

#define HAL_ACC_WRITE_W  0x11000000
#define HAL_ACC_WRITE_HW 0x12000000
#define HAL_ACC_WRITE_B  0x13000000
#define HAL_ACC_READ_W   0x14000000
#define HAL_ACC_READ_HW  0x15000000
#define HAL_ACC_READ_B   0x16000000

#define CALL_APP_FUNCTION 0x60000000  //!< @brief Number of APP Function is coded in the Low Significant Byte
#define CALL_APP_FUNCTION_ERROR 0x60A00000



#define HAL_ACC_WRITE_W_ERROR  0xA0000011
#define HAL_ACC_WRITE_HW_ERROR 0xA0000012
#define HAL_ACC_WRITE_B_ERROR  0xA0000013
#define HAL_ACC_READ_W_ERROR   0xA0000014
#define HAL_ACC_READ_HW_ERROR  0xA0000015
#define HAL_ACC_READ_B_ERROR   0xA0000016



#define ROM1_START 0x00000000
#define ROM1_STOP  0x00002000
#define ROM2_START 0x00002000
#define ROM2_STOP  0x00004000
#define RAM1_START 0x00020000
#define RAM1_STOP  0x00022000
#define RAM2_START 0x20000000
#define RAM2_STOP  0x20002000
#define HW1_START  0x2000A000
#define HW1_STOP   0x2000C800
#define HW2_START  0x20010000
#define HW2_STOP   0x20018C00
#define HW3_START  0x40000000
#define HW3_STOP   0x40000400


/*----------------------------------------------------------------------------
*      Custom data types
 *---------------------------------------------------------------------------*/

/**
 * @brief Enumeration of the different commands supported by the Dandelion
 *        protocol.
 */
typedef enum
{
    DAND_UNKNOWN = 0x00,    //!< enumeration value for unknown commands
    DAND_READ_BYTE = 0x82,    //!< read a byte
    DAND_READ_HALFWORD = 0x81,    //!< read a half word
    DAND_READ_WORD = 0x80,    //!< read a word
    DAND_WRITE_BYTE = 0x72,    //!< write a byte
    DAND_WRITE_HALFWORD = 0x71,    //!< write a half word
    DAND_WRITE_WORD = 0x70,    //!< write a word
    DAND_LOOPBACK = 0xff,    //!< loop back the sent data to the receiver
    DAND_CALL = 0x90,    //!< call a function
    DAND_SEND_MSG = 0x60, /*!< send a message to the Dandelion protocol. This may also call a function.*/ //!< DAND_SEND_MSG
} DAND_CMD_enum_t;

/**
 * @brief Enumeration of the different types of replies of the Dandelion
 *        protocol.
 */
typedef enum
{
    DAND_ACK_CALL_FUNC = 0x90,    //!< Acknowledge call function request.
    DAND_ACK_SEND_MSG = 0x60,    //!< Acknowledge send message request.
    DAND_ACK_READ_WORD = 0x80,    //!< Acknowledge read word request.
    DAND_ACK_READ_HALFWORD = 0x81,    //!< Acknowledge read halfword request.
    DAND_ACK_READ_BYTE = 0x82,    //!< Acknowledge read byte request.
    DAND_ACK_WRITE_WORD = 0x70,    //!< Acknowledge write byte request.
    DAND_ACK_WRITE_HALFWORD = 0x71,    //!< Acknowledge write halfword request.
    DAND_ACK_WRITE_BYTE = 0x72,    //!< Acknowledge write byte request.
    DAND_RLOOPBACK = 0xFF,    //!< Respond to a loopback command.
    DAND_ERROR_REC_CRC = 0xAA,    //!< There was a receiving error (CRC).
    DAND_ERROR_REC_CRCP = 0xA5, //!< There was a receiving error (CRC or Parity).
    DAND_ERROR_READ_PROTEC = 0xC0, //!< The requested address is read-protected.
    DAND_ERROR_WRITE_PROTEC = 0xD0, //!< The requested address is write-protected.
    DAND_ERROR_MIS_ALIGNED = 0xDD,    //!< The address is misaligned.
    DAND_ERROR_UNKNOWN_CMD = 0xAF,    //!< The command is unknown.
    DAND_ERROR_MESSAGE_CALL_DISABLE = 0xED   //!< The requested message/ call execution is disabled
} DAND_Resp_enum_t;

/**
 * @brief Input data structure for the Dandelion protocol.
 *
 * All requests to the Dandelion protocol need to be made through this protocol.
 */
typedef struct
{
    DAND_CMD_enum_t cmd; /*!< Command type requested of the Dandelion protocol.
     The number of bytes of the data-array which are relevant is inferred by
     this value command.*/
    uint8_t data[RX_BUFFER_SIZE]; /*!< An array of unsigned bytes which contains
     the input data for the Dandelion protocol.*/
} rx_DAND_t;


/**
 * @brief Output data structure of the Dandelion protocol.
 *
 * The Dandelion protocol will answer by filling out this data structure.
 */
typedef struct
{
    uint32_t bit_length; /*!< Number of bits in the data array which are to be
     sent back.*/
    uint8_t send_reply; /*!< Whether the contained in the data-array is to be
     sent back to the sender. If zero, no reply is sent. Acts like a boolean.*/
    uint8_t append_check; /*!< Whether the protocol sending the reply should
     append a checksum (like CRC in NFC) to the reply.*/
    uint8_t data[TX_BUFFER_SIZE]; /*!< An array of unsigned bytes which contains
     the output data which the Dandelion protocol wants to have sent back to the
     requester.*/
} tx_DAND_t;

/**
 * @brief Struct which contains the mailbox which can be used to write custom
 * functions which interface with the Dandelion protocol.
 */
typedef struct
{
    uint32_t content[MAILBOX_SIZE];
} Mailbox_t;

/**
 * @brief Function pointer. Defines the interface to which functions have to
 *        adhere so they can be registered with the Dandelion protocol as
 *        callable functions.
 *
 * The type of Mailbox_Fct_Ptr_t is: Pointer to a function which receives
 * a pointer to Mailbox_t and returns a uint32_t.
 * This means functions implementing this interface should look like this:
 * uint32_t function_name(Mailbox_t* mailbox){...}
 * Such a function can be registered with the Dandelion protocol like this:
 * register_function(id, &function_name);
 * The return value of uint32_t is sent back to the requester along with the
 * acknowledgement of the request.
 *
 * @param pointer to the mailbox.
 * @return Data to be appended to the reply of the Dandelion protocol when
 *         acknowledging the function call.
 */
typedef uint32_t (*Mailbox_Fct_Ptr_t)(Mailbox_t*);

/*----------------------------------------------------------------------------
 *      User Interface Function Declarations
 *---------------------------------------------------------------------------*/

/**
 * @brief Initializes the handler for the dandelion protocol by registering
 *        turning on the built-in functions which may be called through the
 *        Dandelion protocol.
 */
extern void init_dand(void);

/**
 * @brief Returns the 32-bit address of element 0 of the mailbox array.
 *
 * The mailbox is a array of <MAILBOX_SIZE> elements of type unsigned integer
 * (32-bit).
 *
 * @return Address of the variable mailbox[0].
 */
extern Mailbox_t* get_mailbox_address(void);

/**
 * @brief Returns the mailbox size.
 *
 * The mailbox size is defined in dand_handler.h as a constant.
 *
 * @return value of constant MAILBOX_SIZE as an unsigned integer (32-bits).
 */
extern uint32_t get_mailbox_size(void);

/**
 * @brief Registers a function so it may be called by the Dandelion protocol.
 *
 * After the function call, the function referred to by the function pointer is
 * associated with the unsigned integer id. This means that whenever the
 * Dandelion protocol is asked to call the function with the number id, the
 * function at the function pointer is executed.
 *
 * @param id ID the function is to be associated with.
 * @param callback_function Pointer to the function which is to be registered
 *        with the Dandelion protocol.
 */
extern void register_function(uint8_t id, Mailbox_Fct_Ptr_t callback_function);

/*----------------------------------------------------------------------------
 *      Protocol Interface Function Declarations
 *---------------------------------------------------------------------------*/
/**
 * @brief Takes in a request to the Dandelion protocol in the form of the
 *        data pointed to by the parameter input. The answer of the Dandelion
 *        protocol is written to the location pointed to by the parameter
 *        output.
 *
 * This function is the interface for all the libraries implementing
 * communication protocols (e.g. NFC, UART) with world outside of the chip.
 * Whenever a message is transmitted through one of these existing protocols
 * which is intended to be interpreted as a Dandelion command, it is to be
 * passed to this function.
 *
 * The dandelion protocol supports the following commands: reading & writing
 * a byte, half-word or word; calling a function; sending a message (which will
 * also initiate a function call); looping back (sending back a received
 * message to the sender [used for debugging purposes]).
 *
 * @param input Pointer to the data structure containing the request to the
 *        Dandelion protocol.
 * @param output Pointer to the data structure into which the reply of the
 *        Dandelion protocol is to be written.
 */
extern void handle_dand_protocol(const rx_DAND_t* input, tx_DAND_t* output);

/**
 * @brief This function resets/wipes the input and output data structures used
 *        to communicate with the Dandelion protocol.
 *
 * @param input Input data structure to be cleared.
 * @param output Output data structure to be cleared.
 */
extern void reset_rx_tx(rx_DAND_t* input, tx_DAND_t* output);

/*----------------------------------------------------------------------------
 *      Internal Function Declarations
 *---------------------------------------------------------------------------*/

/**
 * @brief Returns if the Dandelion protocol may read or write to a specified
 *        address.
 *
 * @param addr Address of interest.
 * @return Whether the Dandelion protocol may read or write to the address
 *         of interest.
 */
extern uint8_t is_legal_addr(const uint32_t addr);

/**
 * @brief Returns the address specified by bytes 1 through 4 of the input data
 *        structure.
 *
 *        It is assumed that the most significant bits of the address are
 *        contained in input->data[1], and the least significant bits are
 *        contained in input->data[4].
 *
 * @param input Input data structure containing an address in bytes 1 through 4.
 * @return Address specified by bytes 1 through 4.
 */
extern uint32_t extract_addr(const rx_DAND_t* input);

/**
 * @brief Function adhering to the Mailbox_Fct_Ptr_t interface registered under
 *        id=0. Calling this function through the Dandelion protocol will return
 *        the mailbox address to the caller of the function.
 *
 * @param None
 * @return Returns the address of the mailbox to be sent back to the sender.
 */
extern uint32_t transmit_mailbox_addr(const Mailbox_t* mailbox);


/**
 * @brief Function adhering to the Mailbox_Fct_Ptr_t interface registered under
 *        id=1. Calling this function through the Dandelion protocol will return
 *        the size of the mailbox as a number of words.
 *
 * @param None
 * @return Size of the mailbox in words.
 */
extern uint32_t transmit_mailbox_size(const Mailbox_t* mailbox);

extern void read_suffix(uint32_t return_code_param, uint32_t value, Mailbox_t* mbox);

/**
 * @brief Function, which is called by external message "mailbox_mnessage_complete"
 *        It will process the message within the mailbox and store the response within the mailbox
 * @param Pointer to mailbox
 * @return returns always 0x0
 */
extern uint32_t mailbox_message_valid(Mailbox_t* mailbox);


/**
 * @brief this function cleans up the mailbox content and sets the mailbox to all Zeros.
 * @param Pointer to mailbox
 * @return returns always 0x0
 */
extern uint32_t purge_mailbox(Mailbox_t* mailbox);

/**
 * @brief this function triggers the programming of an opened NVM page (call nvm_program() in nvm.h)
 * @param mailbox
 * @return return code of nvm_program() function, 0x0 for success
 */
extern uint32_t message_nvm_program(const Mailbox_t* mailbox);

/**
 * @brief this function triggers the erasing of an opened NVM page (call nvm_erase() in nvm.h)
 * @param mailbox
 * @return (0x11000000 | ((uint32_t) nvm_program_page()))
 */
extern uint32_t message_nvm_erase(const Mailbox_t* mailbox);

/**
 * @brief this function switches on the supply of the NVM
 * @param Mailbox
 * @return 0x13
 */
extern uint32_t message_nvm_switch_on(const Mailbox_t* Mailbox);

/**
 * @brief this function switches off the supply of the NVM
 * @param Mailbox
 * @return 0x14
 */
extern uint32_t message_nvm_switch_off(const Mailbox_t* Mailbox);

/**
 * @brief this function opens the addressed NVM page (call nvm_open_assembly_buffer(uint32_t address). The function expects the CPU address in mailbox.content[1]
 * @param Mailbox
 * @return 0x14
 */
extern uint32_t message_nvm_open_assi_buffer(const Mailbox_t* Mailbox);

/**
 * @brief this function calls set_uart_baudrate(uint32_t baudrate) in uart_drv.h
 * @param Mailbox
 * @return 0x4
 */
extern uint32_t message_set_uart_baudrate(const Mailbox_t* Mailbox);

/**
 * @brief this function calls set_singlegpio_out((uint8_t)mailbox.content[1], (uint8_t)mailbox.content[2]);
 *        GPIO number is expected in mailbox.content[1]; value to be set in (uint8_t)mailbox.content[2])
 * @param Mailbox
 * @return 0x15
 */
extern uint32_t message_set_gpio(const Mailbox_t* Mailbox);

/**
 * @brief this function calls get_singlegpio_in((uint8_t) mailbox.content[1])));
 *        GPIO number is expected in mailbox.content[1]
 * @param Mailbox
 * @return (0x16000000 | ((uint32_t) get_singlegpio_in((uint8_t) mailbox.content[1])))
 */
extern uint32_t message_get_gpio(const Mailbox_t* Mailbox);

/**
 * brief     return (0x17000000 |  ((uint32_t) single_gpio_iocfg(
 *                              (bool) ((mailbox.content[1] >> 8)  & 0x1),
 *                             (bool) ((mailbox.content[1] >> 9)  & 0x1),
 *                             (bool) ((mailbox.content[1] >> 10) & 0x1),
 *                             (bool) ((mailbox.content[1] >> 11) & 0x1),
 *                             (bool) ((mailbox.content[1] >> 12) & 0x1),
 *                             (uint8_t) mailbox.content[1])));
 * @param Mailbox
 * @return
 */
extern uint32_t message_single_gpio_cfg(const Mailbox_t* Mailbox);

/**
 * @brief this function calls
 *        set_hb_switch((bool) ((mailbox.content[1] >> 24) & 0x1),
 *                      (bool) ((mailbox.content[1] >> 16) & 0x1),
 *                      (bool) ((mailbox.content[1] >> 8) & 0x1),
 *                      (bool) ((mailbox.content[1] >> 0) & 0x1));
 *        arguments to function are provided in mailbox.content[1]
 * @param Mailbox
 * @return 0x5
 */
extern uint32_t message_ctrl_hbridge(const Mailbox_t* Mailbox);

/**
 * @brief this function switches the NVM into default read mode, it calls nvm_config()
 * @param Mailbox
 * @return 0x9
 */
extern uint32_t message_nvm_normal_read(const Mailbox_t* Mailbox);

/**
 * @brief this function calls sense_sh_lp(true, true, 0x4, 0x5)
 * @param Mailbox
 * @return ((uint32_t)tmp.sh_result[0]) | ((uint32_t) tmp.sh_result[1] << 16)
 */
extern uint32_t message_sense_adc(const Mailbox_t* Mailbox);

/**
 * @brief this function calls (uint32_t) get_nfc_value(rssi), reads out the NFC RSSI status
 * @param Mailbox
 * @return (uint32_t) get_nfc_value(rssi)
 */
extern uint32_t message_get_rssi(const Mailbox_t* Mailbox);

/**
 * @brief this function calls (uint32_t) get_nfc_value(vdd_ca)
 * @param Mailbox
 * @return (uint32_t) get_nfc_value(vdd_ca)
 */
extern uint32_t message_get_vccca(const Mailbox_t* Mailbox);


#ifdef __cplusplus
}
#endif

/** @} */ /* End of group dand_handler */

/** @} */ /* End of group Smack */

/** @} */ /* End of group Infineon */

#endif /* _DAND_HANDLER_H_ */

