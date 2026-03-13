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
 * @file     nvm_lib.h
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


#ifndef _NVM_LIB_H_
#define _NVM_LIB_H_

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


/*  Remark regarding linker settings:
 *
 *  The function nvm_program_verify_lib() temporarily modifies the NVM settings and must be executed from RAM.
 *  This function is located into a separate section, with a name starting with ".ramtest". This section shall
 *  be located in RAM for save operation, as opcode fetches from NVM may not be reliable during the verify.
 *
 *  Please note that nvm_program_page_lib() is calling nvm_program_verify_lib().
 *
 *  Do not mix functions from this library with the one of the ROM library to write to NVM.
 */

/**
 * @brief Smack NVM Parameters
 *
 * These are defined in nvm.h and provided here in case this file is not included.
 */

#ifndef NUMBER_OF_SECTORS
#define NUMBER_OF_SECTORS 15 //!< a sector contains 32 pages
#endif
#ifndef N_BLOCKS
#define N_BLOCKS          16 //!< a block contains two (32-bit) words
#endif
#ifndef N_LOG_PAGES
#define N_LOG_PAGES       32 //!< a page contains 16 blocks
#endif
#ifndef N_PHYS_PAGES
#define N_PHYS_PAGES      33
#endif

#ifndef NVM_BASE
#define NVM_BASE 0x00010000 //!< NVM base address (NVM start), means lowest NVM address
#endif
#ifndef NVM_SIZE
#define NVM_SIZE 0x0000F000 //!< 60kByte
#endif
#ifndef NVM_STOP
#define NVM_STOP (NVM_BASE + NVM_SIZE) //!< nvm stop, means highest NVM Address
#endif


typedef struct
{
    uint8_t busy_wait : 1;      // use busy wait loop instead of SysTick based delay with WFI
} nvm_config_lib_t;


/**
 * @brief This function switches on the NVM and controls the proper power up of the NVM. The NVM has a separate own internal supply.
 * @param None
 * @return None
 */
extern void nvm_power_on_lib(void);

/**
 * @brief This function switches off the NVM and controls the safe power down of the NVM. The NVM has a separate own internal supply.
 * @param None
 * @return None
 */
extern void nvm_power_off_lib(void);

/**
 * @brief This function sets the configuration of the NVM driver.
 * @param  config: struct with configuration data
 * @return None
 */
extern void nvm_config_set_lib(const nvm_config_lib_t config);

/**
 * @brief This function sets the NVM into read mode. The NVM is idle and waits for read access.
 * @param None
 * @return None
 */
extern void nvm_mode_normal_lib(void);

// Routines acc. to recommandation chapter 1.13.4 of SIFO document

/**
 * @brief This function opens the assembly buffer, which means the page related to uint32_t cpu_address is copied into assembly buffer.
 *        A read access to the page will deliver the addressed /word/halfword/byte by the content of the assembly buffer.
 *        A write access to the page will change the addressed /word/halfword/byte in the assembly buffer.
 * @param cpu_address
 * @return an 8-bit unsigned integer. value 0x0 indicates correct opening
 */
extern uint8_t nvm_assembly_buffer_open_lib(void* cpu_address);

/**
 * @brief This function performs a programming of a page, with the content of  assembly buffer
 *        It is calling the functions nvm_erase_page(), nvm_program_page().
 *        This function should be used in application, when programming a page.
 * @param None
 * @return an 8-bit unsigned integer. value 0x0 indicates correct programming
 */
extern uint8_t nvm_program_page_lib(void);

/**
 * @brief This function performs a programming and verification of the programming result of a nvm page, which is opened in assembly buffer
 *        It is calling the functions nvm_erase_page(), nvm_program_page().
 *        This function should be used in application, when programming a page.
 * @param None
 * @return an 8-bit unsigned integer. value 0x0 indicates correct programming
 */
extern uint8_t nvm_program_verify_page_lib(void);

/**
 * @brief This function aborts a running programming of an opened page in assembly buffer. It will set the nvm into idle and wait for read.
 * @param None
 * @return None
 */
extern void nvm_program_abort_lib(void);

/**
 * @brief This function erases the nvm page, which is opened in the assembly buffer. As the result of erasing, all bits of the NVM page are set to High.
 * @param None
 * @return None
 */
extern void nvm_erase_page_lib(void);


// compatibility with legacy prototypes
static inline void switch_on_nvm_lib(void)
{
    nvm_power_on_lib();
}
static inline void switch_off_nvm_lib(void)
{
    nvm_power_off_lib();
}
static inline void nvm_config_lib(void)
{
    nvm_mode_normal_lib();
}
static inline uint8_t nvm_open_assembly_buffer_lib(void* cpu_address)
{
    return nvm_assembly_buffer_open_lib(cpu_address);
}
static inline uint8_t nvm_program_verify_lib(void)
{
    return nvm_program_verify_page_lib();
}
static inline void nvm_abort_program_lib(void)
{
    nvm_program_abort_lib();
}


#ifdef __cplusplus
}
#endif

/** @} */ /* End of group NVM */


/** @} */ /* End of group Smack */

/** @} */ /* End of group Infineon */

#endif /* _NVM_LIB_H_ */
