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
 * @file     aes_lib.h
 *
 * @brief    This module extends the aes_drv driver of the ROM library.
 *
 * @version  v1.0
 * @date     2021-08-01
 *
 */

#ifndef AES_LIB_H_
#define AES_LIB_H_

/** @addtogroup Infineon
 * @{
 */

/** @addtogroup Smack
 * @{
 */


/** @addtogroup aes_lib
 * @{
 */

#include "aes_drv.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief  A union to store one block of AES128 data, accessible either as byte or word array
 */
typedef union
{
    uint8_t  b[16];     // 16 bytes (128 bits) -> byte stream
    uint32_t w[4];      // 4 words of 4 bytes (128 bits) -> words in network byte order
} aes_block_t;


/**
 * @brief  This function loads a 128-bit wide key into the AES hardware accelerator and performs the calculation of round
 *         keys for decryption type of operation
 *
 *         This is a clone of the aes_load_key() function of the ROM library which takes byte streams as an argument instead
 *         of uint32 arrays in host byte order.
 * @param  key, pointer to a byte array which holds the AES key
 * @return None
 */
extern void aes_load_key_ba(const aes_block_t* key);

/**
 * @brief  This function calculates a 128-bit AES decryption or encryption operation.
 *
 *         The calculated 128-bit wide result of operation will be stored in an array of 16 bytes by the function.
 *         The input data is also taken from an array of 16 bytes which may be a part of a data stream.
 *         The pointers to input data and result can be the same if input data is not needed anymore after the operation.
 *         The calculation of an 128-bit operation is hardware accelerated and takes 16 processor cycles.
 *         It assumes, that a valid key was loaded before into the hardware accelerator using function aes_load_key(const uint32_t* key).
 *
 *         This is a clone of the calc_aes() function of the ROM library which takes byte streams as arguments instead
 *         of uint32 arrays in host byte order.
 * @param  result  pointer to a byte array receiving the result
 * @param  data    pointer to a byte arry holding the input data
 * @param  op_type aes_operation_type defines the type of operation. Can be of value "encrypt" or "decrypt
 * @return None
 */
extern void calc_aes_ba(aes_block_t* result, const aes_block_t* data, aes_operation_type op_type);


/**
 * @file
 * @brief  Random number generators
 *
 * The ROM and NVM libraries provide different functions to generate true or pseudo random numbers.
 * The quality of the true random number generators depends on the amount of random seed used
 * in the different functions which also determines their execution time.
 *
 * The base for the random number generators is the AES unit of the chip. By encrypting a start
 * code a result code is generated that is used as the start code of the next step. This way, a
 * sequence of pseudo random numbers is generated.
 *
 * On the initial call of a random number generator the first start code is built from the UID of
 * the chip and some other data gathered from the digital part of the device. The sequence of pseudo
 * random numbers will be different between the chips but be the same for different life cycles of
 * the same chip.
 *
 * To convert the scheme described above to produce true random numbers some of the functions pick
 * up noise from the analog circuit of the chip. This process takes some time and may not be suitable
 * in every application. Therefore, several functions are provided that are using a different number
 * of rounds to retrieve the noise so that firmware developers can choose between better distribution
 * of the random numbers, or faster execution.
 *
 * The random number generators provided in this file are using the same buffer for the start code
 * and can be combined. So, to get a good distribution of the random numbers, first a slow function
 * with a high amount of noise can be used to generate an unpredictable true random number, then
 * for further numbers, the fastest function can be called that calculates a pseudo random number
 * that is unpredictable because of the true random start value.
 *
 * The retrieved noise returns an entropy of about 4 bits. When recording an infinite count of
 * random numbers generated after startup of the device using the function that does one round of
 * noise retrieval, this will result in somewhat between 15 and 20 unique random numbers, all others
 * will be duplicates of them. The more rounds of noise retrieval is used, the more distinctive
 * random numbers will be returned but the amount of time to generate them will increase.
 *
 * To give some hints of the performance of the functions, a test was performed that prints out
 * a random number after startup of the chip, then reboots. To evaluate the results, the first
 * 32 bits of the return values were recorded and compared. After 5.4 million cycles, the RNG
 * function with one round of analog noise retrieval returned 16 unique numbers with the others
 * being dupes, with two rounds of noise there were 892 distinctive numbers, and by using a seed
 * with 16 rounds of analog noise all but 5968 random numbers were unique. The amount of dupes is
 * 0.11% which is almost exactly the statistical probability of dupes an a 5.4 million test vector
 * within the range of a 32-bit integer.
 * Picking up random noise takes about 59us per round.
 *
 * Functions that can be used alternating to select between faster execution time or more random
 * seed:
 * - rand_lib()
 * - generate_random_number_fast()
 * - generate_random_number_lib()
 * - generate_random_number_slow16_lib()
 * - generate_random_number_slow8_lib()
 * - generate_random_number_slow4_lib()
 * - generate_random_number_slow2_lib()
 * - generate_random_number_slow1_lib()
 */

/**
 * @brief  Calculate a pseudo random number using the AES unit. No analog noise is used as a seed.
 *         This function uses the same calculation as generate_random_number_fast().
 * @return first 32 bits of the 128-bit result of the AES unit
 */
extern uint32_t rand_lib(void);

/**
 * @brief  Calculate a pseudo random number using the AES unit. No analog noise is used as a seed.
 * @param  random_number pointer to 128-bit struct were the random number is stored
 */
extern void generate_random_number_fast(aes_block_t* random_number);

/**
 * @brief  Calculate a random number seeded with 1 round of analog noise.
 *         The distribution of the seed in the plain text passed to the AES unit is different from
 *         the one used in generate_random_number_slow1_lib(), so both functions will produce
 *         different results.
 * @param  random_number pointer to 128-bit struct were the random number is stored
 */
extern void generate_random_number_lib(aes_block_t* random_number);

/**
 * @brief  Calculate a random number seeded with 16 rounds of analog noise
 * @param  random_number pointer to 128-bit struct were the random number is stored
 */
extern void generate_random_number_slow16_lib(aes_block_t* random_number);

/**
 * @brief  Calculate a random number seeded with 8 rounds of analog noise
 * @param  random_number pointer to 128-bit struct were the random number is stored
 */
extern void generate_random_number_slow8_lib(aes_block_t* random_number);

/**
 * @brief  Calculate a random number seeded with 4 rounds of analog noise
 * @param  random_number pointer to 128-bit struct were the random number is stored
 */
extern void generate_random_number_slow4_lib(aes_block_t* random_number);

/**
 * @brief  Calculate a random number seeded with 2 rounds of analog noise
 * @param  random_number pointer to 128-bit struct were the random number is stored
 */
extern void generate_random_number_slow2_lib(aes_block_t* random_number);

/**
 * @brief  Calculate a random number seeded with 1 round of analog noise
 * @param  random_number pointer to 128-bit struct were the random number is stored
 */
extern void generate_random_number_slow1_lib(aes_block_t* random_number);


//=================================================================

// not a reset but initialization with defaults
//extern void sense_reset(void);

#ifdef __cplusplus
}
#endif

/** @} */ /* End of group aes_lib */

/** @} */ /* End of group Smack */

/** @} */ /* End of group Infineon */

#endif /* AES_LIB_H_ */
