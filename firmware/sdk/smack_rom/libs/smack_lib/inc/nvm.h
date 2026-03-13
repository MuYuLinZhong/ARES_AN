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
 * @file     nvm.h
 *
 * @brief    Definitions for the Smack NVM.
 *           The offset parameter definitions define the offset address to NVM start for NVM parameter fields
 *           This parameter fields contain system-, application-, production- data of the chip.
 *
 * @version  v1.0
 * @date     2020-05-20
 *
 * @note
 */

/* lint -save -e960 */


#ifndef _NVM_H_
#define _NVM_H_

/** @addtogroup Infineon
 * @{
 */

/** @addtogroup Smack
 * @{
 */


/** @addtogroup NVM
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

#define NVM_LDO_SETTLE 0x000000FF //ca. 5us

/**
 * @brief Smack NVM Parameters
 */
#define NUMBER_OF_SECTORS 15 //!< a sector contains 32 pages
#define N_BLOCKS          16 //!< a block contains two (32-bit) words
#define N_LOG_PAGES       32 //!< a page contains 16 blocks
#define N_PHYS_PAGES      33

#define NVM_BASE 0x00010000 //!< NVM base address (NVM start), means lowest NVM address
#define NVM_SIZE 0x0000F000 //!< 60kByte
#define NVM_STOP (NVM_BASE + NVM_SIZE) //!< nvm stop, means highest NVM Address + 1 (first address beyond NVM)


/**
 * @brief This function switches on the NVM and controls the proper power up of the NVM. The NVM has a separate own internal supply.
 * @param None
 * @return None
 */
extern void switch_on_nvm(void);

/**
 * @brief This function switches off the NVM and controls the safe power down of the NVM. The NVM has a separate own internal supply.
 * @param None
 * @return None
 */
extern void switch_off_nvm(void);

/**
 * @brief This function sets the NVM into read mode. The NVM is idle and waits for read access.
 * @param None
 * @return None
 */
extern void nvm_config(void);

// Routines acc. to recommandation chapter 1.13.4 of SIFO document

/**
 * @brief This function opens the assembly buffer, which means the page related to uint32_t cpu_address is copied into assembly buffer.
 *        A read access to the page will deliver the addressed /word/halfword/byte by the content of the assembly buffer.
 *        A write access to the page will change the addressed /word/halfword/byte in the assembly buffer.
 * @param cpu_address
 * @return an 8-bit unsigned integer. value 0x0 indicates correct opening
 */
extern uint8_t nvm_open_assembly_buffer(uint32_t cpu_address);

/**
 * @brief This function performs a programming of a page, with the content of  assembly buffer
 *        It is calling the functions nvm_erase_page(), nvm_program_page().
 *        This function should be used in application, when programming a page.
 * @param None
 * @return an 8-bit unsigned integer. value 0x0 indicates correct programming
 */
extern uint8_t nvm_program_page(void);

/**
 * @brief This function performs a verification of the programming result of a nvm page, which is opened in assembly buffer
 *        This function should be used in application, when programming a page.
 * @param None
 * @return an 8-bit unsigned integer. value 0x0 indicates correct programming
 */
extern uint8_t nvm_program_verify(void);

/**
 * @brief This function aborts a running programming of an opened page in assembly buffer. It will set the nvm into idle and wait for read.
 * @param None
 * @return None
 */
extern void nvm_abort_program(void);

/**
 * @brief This function erases the nvm page, which is opened in the assembly buffer. As the result of erasing, all bits of the NVM page are set to High.
 * @param None
 * @return None
 */
extern void nvm_erase_page(void);

/**
 * @brief this is the IRQ routine of NVM IRQ.
 */
extern void serve_nvmirq(void);

/**
 * @brief Definition of access types to access a NVM page, when accessed via external comm interface (e.g. UART, NFC)
 */
typedef enum access_state_e
{
    read_write,//!< read_write
    read_only, //!< read_only
    write_only,//!< write_only
    no_access  //!< no_access
} access_state_t;

/**
 * @brief This function returns back the access privilegs of a addressed NVM page, when accessed from external interface. It is used in dand_handler.c
 *        The function extracts the NVM logical sector/page address from parameter address, reads the access state of that page from NVM and returns the access state.
 * @param address
 * @return can be of value read/write, read only, write only, no_access
 */
extern access_state_t get_nvm_access_state(uint32_t address);


#ifdef __cplusplus
}
#endif

/** @} */ /* End of group NVM */


/** @} */ /* End of group Smack */

/** @} */ /* End of group Infineon */

#endif /* _NVM_H_ */
