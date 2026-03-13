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
 * @file     aes_drv.h
 *
 * @brief    Driver for the AES encryption block.
 *
 * @version  v1.0
 * @date     2020-05-20
 *
 * @note
 */

/*lint -save -e960 */

#ifndef _AES_DRV_H_
#define _AES_DRV_H_


/** @addtogroup Infineon
 * @{
 */

/** @addtogroup Smack
 * @{
 */


/** @addtogroup aes
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Definition of the AES operation Type
 */
typedef enum aes_operation_e
{
    encrypt, //!< Encryption Operation
    decrypt //!< Decryption Operation Type
} aes_operation_type;


/**
 * @brief This function calculates the round keys of a AES decryption operation for a given key, which is loaded into the HW accelerator.
 *        It is e.g. used in aes_load_key(...),  calc_aes(...)
 * @param None
 * @return None
 */
extern void run_pnk(void);

/**
 * @brief  This stalls the cpu execution for the duration of a complete AES encryption/ decryption calcualtion.
 *         Is e.g. used inside run_pnk(), calc_aes(...)
 * @param None
 * @return None
 */
extern void check_aes_busy(void);

/**
 * @brief  This function loads a 128-bit wide key into the AES hardware accelerator and performs the calculation of round
 *         keys for decryption type of operation
 * @param key, pointer to the element 0 of an array of 4 32-bit integers, which contain the AES key
 * @return None
 */
extern void aes_load_key(const uint32_t* key);

/**
 * @brief This function calculates a 128-bit AES decryption or encryption operation.
 *        The calculated 128-bit wide result of operation will be stored in an array of 4 32-bit integers by the function
 *        , whereas the pointer to  element 0 is an input parameter to the function.
 *        The input data of operation is given in an array of 4 32-bit integers to the function, whereas the pointer to element 0 is argument to the function.
 *        The calculation of an 128-bit operation is hardware accelerated and takes 16 processor cycles.
 *        It assumes, that a valid key was loaded before into the hardware accelerator using function aes_load_key(const uint32_t* key).
 * @param result  uint32_t * pointer to the element 0 of result array
 * @param data uint32_t * pointer to the element 0 of input data array
 * @param op_type aes_operation_type defines the type of operation. Can be of value "encrypt" or "decrypt
 */
extern void calc_aes(uint32_t* result, const uint32_t* data, aes_operation_type op_type);



/**
 * @brief This function generates a 128-bit true random number. It requires read access to the NVM and the power up of sensing unit.
 *        The generated 128-bit random number is stored in an array of 4 32-bit, whereas the pointer to  element 0 is an input parameter to the function.
 * @param random_number A pointer to a 32-bit integer is provided. This pointer  is interpreted as the pointer to element 0, of an array of 4 32-bit integers. The function
 *        writes the generated true random number into  4 consecutive word addresses of the memory, starting from the address the parameter pointer is pointing to.
 * @return None
 */
extern void generate_random_number(uint32_t* random_number);


/**
 * @brief TRQ service request routine for AES interrupt. Called in respective IRQ handler
 */
extern void serve_aes_irq(void);

#ifdef __cplusplus
}
#endif

/** @} */ /* End of group aes block */

/** @} */ /* End of group Smack */

/** @} */ /* End of group Infineon */

#endif /* _AES_DRV_H_ */
