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
 * @file     nfc_handler.h
 *
 * @brief    NFC handler.
 *
 * @version  v1.0
 * @date     2020-05-20
 *
 * @note
 */

/*lint -save -e960 */

#ifndef _NFC_HANDLER_H_
#define _NFC_HANDLER_H_

#include "dand_handler.h"

/** @addtogroup Infineon
 * @{
 */

/** @addtogroup Smack
 * @{
 */


/** @addtogroup nfc_handler
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif


/*----------------------------------------------------------------------------
*      Custom data types
 *---------------------------------------------------------------------------*/

/**
 * @brief Enumeration of states as defined in ISO-14443-3.
 */
typedef enum
{
    NFC_IDLE = 0,    //!< IDLE state
    NFC_READY_0 = 1,    //!< READY state
    NFC_ACTIVE_0 = 2,    //!< ACTIVE state
    NFC_HALT = 3,    //!< HALT state
    NFC_READY_1 = 4, //!< READY* state
    NFC_ACTIVE_1 = 5, //!< ACTIVE* state
    NFC_PROT_DAND = 6  //!< proprietary state not conforming to 14443-4
} NFC_State_enum_t;

/**
 * @brief Enumeration of NFC commands as defined in ISO-14443-3.
 *
 * The assigned values are equal to the value of the first received byte in
 * frames encoding the respective command.
 */
typedef enum
{
    NFC_UNKNOWN = 0x00,    //!< Unknown command.
    NFC_REQA = 0x26,    //!< REQA command (short frame)
    NFC_WUPA = 0x52,    //!< WUPA command (short frame)
    NFC_Timeslot = 0x35,    //!< Timeslot command (short frame)
    NFC_HLTA = 0x50,    //!< HLTA command (standard frame)
    NFC_SEL_1 = 0x93,    //!< SEL1 command (anticollision frame cascade level 1)
    NFC_SEL_2 = 0x95,    //!< SEL2 command (anticollision frame cascade level 2)
    NFC_SEL_3 = 0x97,    //!< SEL3 command (anticollision frame cascade level 3)
    // custom commands
    NFC_RATS_DAND = 0x47, //!< proprietary command: enters PROT_DAND state (standard frame)
    NFC_DESELECT_DAND = 0x48, //!< proprietary command: exits PROT_DAND state (standard frame)
    NFC_DAND_CMD = 0x01, /*!< proprietary command: frames marked with this command value
     contain a proprietary Dandelion command (standard frame).*/
    NFC_T2T_READ = 0x30 //!< NFC Type 2 Tag Read
} NFC_Cmd_enum_t;

/**
 * @brief Enumeration of the possible responses the NFC protocol may return.
 */
typedef enum
{
    NFC_NO_RESPONSE = 0x00, //!< no response at all.
    NFC_ATQA = 0x42, //!< ATQA (answer to REQA).
    NFC_SAK = 0x04, //!< SAK (select acknowledge)
    NFC_UID_PARTIAL = 0x93,    //!< only transmit part of the UID.
    NFC_ACK_RATS_DAND = 0x47,    //!< Acknowledge RATS_DAND
    NFC_ERROR_REC_CRC = 0xAA,    //!< Receive Error (CRC only)
    NFC_ERROR_REC_CRCP = 0xA5,    //!< Receive Error (CRC + Parity)
    NFC_ERROR_READ_PROTEC = 0xC0,    //!< Address is read-protected
    NFC_ERROR_WRITE_PROTEC = 0xD0,    //!< Address is write-protected
    NFC_ERROR_MIS_ALIGNED = 0xDD,    //!< Address is mis-aligned
    NFC_ERROR_UNKNOWN_CMD = 0xAF //!< Bad command received while in PROT_DAND state
} NFC_Resp_enum_t;

/**
 * @brief Enumeration of the frame types which may be received according to
 *        ISO-14443-3 Section 6.2.3.
 */
typedef enum
{
    NFC_ERROR = 0,      //!< any erroneous frame formats
    NFC_SHORT,          //!< length = 7 bits
    NFC_STANDARD,       //!< length = nx8 bits
    NFC_ANTICOLLISION   //!< length = x with 16 bits <= x <= 48 bits
} NFC_Frame_enum_t;

/**
 * @brief Input data structure for the NFC protocol.
 *
 * All received data is parsed and placed into this struct by the read_frame
 * function.
 */
typedef struct
{
    uint32_t len; //!< amount of bits in the frame.
    NFC_Frame_enum_t type; //!< type of the frame (short, normal, anticollision).
    NFC_Cmd_enum_t cmd; //!< command contained by the frame.
    uint8_t p_error; //!< whether there was a parity error when receiving the frame.
    rx_DAND_t dand_rx; //!< data structure to interface with the dand_handler.h library.
} NFC_Frame_t;

/**
 * @brief Initializes the NFC driver.
 *
 * Needs to be called once during the program start in order to be able to
 * receive NFC commands.
 */
extern void nfc_init(void);

/*----------------------------------------------------------------------------
*      Actual Handlers
 *---------------------------------------------------------------------------*/
/**
 * @brief Reads the bytes received from the CLUART into the NFC_Frame_t struct,
 *        checks the received data for errors (parity error), and stores the
 *        length of the received data.
 */
extern void read_frame (void);

/**
 * @brief Classifies the type of frame that has been received.
 *
 * There are three types of commands according to ISO-14443-3 sec. 6.2.3:
 * Short frames: 7 bits
 * Standard frames: multiples of 8 bits long (with parity bits between the bytes)
 * Anticollision frames: Between 16 and 48 bits long.
 *
 * @return Type of the received frame.
 */
extern NFC_Frame_enum_t classify_frame(void);

/**
 * @brief Classifies the command of the received command.
 *
 * In case the command is of type NFC, it is classified as such and this is
 * returned.
 * In case the command is a Dandelion command, it is classified as NFC_DAND_CMD
 * (this is the return value), and the type of dandelion command is stored in
 * frame.dand_rx.cmd.
 *
 * @return Type of command from the perspective of the NFC-protocol.
 */
extern NFC_Cmd_enum_t classify_command (void);

/**
 * @brief Resets the CRC calculating part of the CLUART.
 */
extern void reset_CRC_peripheral (void);

/**
 * @brief Resets the return_frame to its default state: Nothing is being
 *        returned, no CRC is appended to the message, and the bit-length is
 *        zero.
 */
extern void clear_ret_frame (void);

/**
 * @brief Returns whether the CRC of the received message is correct (0 if not,
 *        non-zero otherwise).
 *
 * @return Whether the CRC/checksum of the received message was correct.
 */
extern uint8_t is_CRC_A_correct (void);

/**
 * @brief Returns whether this chip was selected by the Select-command sent
 *        by the PCD (0 if not, non-zero otherwise). This has to do with how
 *        the NFC-protocol implements anticollision.
 *
 * @return Whether this PICC/chip is being selected by the PCD.
 */
extern uint8_t is_selected (void);

/**
 * @brief Sets up the hardware peripherals to send the reply generated by the
 *        protocols back to the PCD.
 */
extern void send_return_frame (void);

/**
 * @brief This function implements the anticollision algorithm as specified by
 *        ISO-14443-3.
 *
 * There are three possible outcomes of this function:
 * a) this chip/PICC is not selected -> the anticollision procedure is
 *    terminated and the state is set to IDLE or HALT.
 * b) this chip/PICC has been fully selected -> the state gets promoted to
 *    either ACTIVE or ACTIVE*.
 * c) this chip/PICC has not been fully selected -> the state stays in READY
 *    or READY* state.
 *
 * @return New state of the chip/PICC (IDLE/HALT,READY/READY*,ACTIVE/ACTIVE*).
 */
extern NFC_State_enum_t handle_anticollision (void);

/**
 * @brief This function handles the interaction of the NFC protocol with the
 *        Dandelion protocol, and returns the new state of the chip/PICC (either
 *        it stays in the PROT_DAND state, or it gets demoted to HALT).
 *
 * The chip may be deselected through either a DESELECT_DAND or HALT command.
 * In this case the state is demoted to HALT.
 *
 * In case the chip is not deselected, the command is passed on to the
 * dand_handler.h library, and this function just uses the reply that is being
 * returned. The state of the chip/PICC stays PROT_DAND (this is the return
 * value).
 *
 * @return New state of the chip/PICC (HALT, PROT_DAND).
 */
extern NFC_State_enum_t handle_DAND_protocol (void);

/**
 * @brief Sets up the reply data structure to communicate a parity error.
 */
extern void send_p_error (void);

/**
 * @brief Implements the state machine as defined in ISO-14443-3.
 *
 * This function updates the state of the PICC/chip according to the input
 * messages it receives. Depending on this state the input messages are passed
 * on to different handling functions such as handle_DAND_protocol, or
 * handle_anticollision. In the end the goal is to reply to all incoming
 * messages in accordance with the NFC protocol defined in ISO-14443-3.
 */
extern void nfc_state_machine (void);

/**
 * @brief Resets the data structure used to communicate with the external uDMA
 *        hardware.
 */
extern void cleanup_routine(void);

/**
 * @brief irq service request function, in case a loss of RF field is detected. It resets the NFC state to IDLE and reinitializes the NFC HW/SW.
 */
extern void serve_hw_field_off_irq(void);

#ifdef __cplusplus
}
#endif

/** @} */ /* End of group nfc_handler */


/** @} */ /* End of group Smack */

/** @} */ /* End of group Infineon */

#endif /* _NFC_HANDLER_H_ */
