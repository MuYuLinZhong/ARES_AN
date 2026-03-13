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

/*
 * ssp_drv.h
 *
 *  Created on: 30.07.2020
 *      Author: schnemat
 */

#ifndef LIBS_SMACK_LIB_INC_SSP_DRV_H_
#define LIBS_SMACK_LIB_INC_SSP_DRV_H_

#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef enum ssp_ifmode_e
{
    ssp_master, //!< interface master
    ssp_slave   //!< interface slave
} ssp_ifmode_t;

typedef enum ssp_phase_e  //!< see page 2.12 of SSP Databook ARM DDI 0194D to understand the meaning of the bit
{
    ssp_first,   //!< data captured on the first edge of sspclk, when CS is acctive
    ssp_second   //!< clock captured on the second edge of sspclk, when CS is active
} ssp_phase_t;

typedef enum ssp_polarity_e
{
    ssp_high, //!< SSPCLKOUT pin of SPI is HIGH in steady state
    ssp_low   //!< SSPCLKOUT pin of SPI is LOW in steady state
} ssp_polarity_t;

extern void control_spi(bool en_dis, bool loop_back);

extern void config_spi(uint8_t clockrate, ssp_ifmode_t mode, uint8_t number_of_bits, ssp_polarity_t clock_polarity, ssp_phase_t clock_phase);

extern void config_ssp_irqs(bool txim, bool rxim, bool rtim, bool rorim, uint8_t irq_num);

extern void serve_ssp_irq(uint8_t irq_num);

#ifdef __cplusplus
}
#endif

#endif /* LIBS_SMACK_LIB_INC_SSP_DRV_H_ */
