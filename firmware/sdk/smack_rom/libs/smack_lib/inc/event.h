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
 * @file     event.h
 *
 * @brief    Declaration of Smack event codes according to event_codes.xls
 *
 * @version  v1.0
 * @date     2020-05-20
 *
 * @note
 */

/*lint -save -e960 */

#ifndef _EVENT_H_
#define _EVENT_H_

/** @addtogroup Infineon
 * @{
 */

/** @addtogroup Smack
 * @{
 */


/** @addtogroup event
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Event Codes (16-Bit)
 */
#define IDLE_IDLE                      (0b0000000000000000) //!< Event Idle Code

#define INTERRUPT_REQUEST_IRQ0         (0b0000010000000000) //!< Event IRQ0 Request
#define INTERRUPT_REQUEST_IRQ1         (0b0000010000000001) //!< Event IRQ1 Request
#define INTERRUPT_REQUEST_IRQ2         (0b0000010000000010) //!< Event IRQ2 Request
#define INTERRUPT_REQUEST_IRQ3         (0b0000010000000011) //!< Event IRQ3 Request
#define INTERRUPT_REQUEST_IRQ4         (0b0000010000000100) //!< Event IRQ4 Request
#define INTERRUPT_REQUEST_IRQ5         (0b0000010000000101) //!< Event IRQ5 Request
#define INTERRUPT_REQUEST_IRQ6         (0b0000010000000110) //!< Event IRQ6 Request
#define INTERRUPT_REQUEST_IRQ7         (0b0000010000000111) //!< Event IRQ7 Request
#define INTERRUPT_REQUEST_IRQ8         (0b0000010000001000) //!< Event IRQ8 Request
/*
   #define INTERRUPT_REQUEST_IRQ9         (0b0000010000001001)
   #define INTERRUPT_REQUEST_IRQ10        (0b0000010000001010)
   #define INTERRUPT_REQUEST_IRQ11        (0b0000010000001011)
   #define INTERRUPT_REQUEST_IRQ12        (0b0000010000001100)
   #define INTERRUPT_REQUEST_IRQ13        (0b0000010000001101)
   #define INTERRUPT_REQUEST_IRQ14        (0b0000010000001110)
   #define INTERRUPT_REQUEST_IRQ15        (0b0000010000001111)
   #define INTERRUPT_REQUEST_IRQ16        (0b0000010000010000)
   #define INTERRUPT_REQUEST_IRQ17        (0b0000010000010001)
   #define INTERRUPT_REQUEST_IRQ18        (0b0000010000010010)
   #define INTERRUPT_REQUEST_IRQ19        (0b0000010000010011)
   #define INTERRUPT_REQUEST_IRQ20        (0b0000010000010100)
   #define INTERRUPT_REQUEST_IRQ21        (0b0000010000010101)
   #define INTERRUPT_REQUEST_IRQ22        (0b0000010000010110)
   #define INTERRUPT_REQUEST_IRQ23        (0b0000010000010111)
   #define INTERRUPT_REQUEST_IRQ24        (0b0000010000011000)
   #define INTERRUPT_REQUEST_IRQ25        (0b0000010000011001)
   #define INTERRUPT_REQUEST_IRQ26        (0b0000010000011010)
   #define INTERRUPT_REQUEST_IRQ27        (0b0000010000011011)
   #define INTERRUPT_REQUEST_IRQ28        (0b0000010000011100)
   #define INTERRUPT_REQUEST_IRQ29        (0b0000010000011101)
   #define INTERRUPT_REQUEST_IRQ30        (0b0000010000011110)
   #define INTERRUPT_REQUEST_IRQ31        (0b0000010000011111)
*/
#define INTERRUPT_REQUEST_NMI          (0b0000010000100000) //!< Event NMI Request

#define DMA_REQUEST_CHANNEL0           (0b0000100000000000) //!< Event DMA Channel 0 Transfer
#define DMA_REQUEST_CHANNEL1           (0b0000100000000001) //!< Event DMA Channel 1 Transfer
#define DMA_REQUEST_CHANNEL2           (0b0000100000000010) //!< Event DMA Channel 2 Transfer
#define DMA_REQUEST_CHANNEL3           (0b0000100000000011) //!< Event DMA Channel 3 Transfer
#define DMA_REQUEST_CHANNEL4           (0b0000100000000100) //!< Event DMA Channel 4 Transfer
#define DMA_REQUEST_CHANNEL5           (0b0000100000000101) //!< Event DMA Channel 5 Transfer
#define DMA_REQUEST_CHANNEL6           (0b0000100000000110) //!< Event DMA Channel 6 Transfer
#define DMA_REQUEST_CHANNEL7           (0b0000100000000111) //!< Event DMA Channel 7 Transfer
/*
   #define DMA_REQUEST_CHANNEL8           (0b0000100000001000)
   #define DMA_REQUEST_CHANNEL9           (0b0000100000001001)
   #define DMA_REQUEST_CHANNEL10          (0b0000100000001010)
   #define DMA_REQUEST_CHANNEL11          (0b0000100000001011)
   #define DMA_REQUEST_CHANNEL12          (0b0000100000001100)
   #define DMA_REQUEST_CHANNEL13          (0b0000100000001101)
   #define DMA_REQUEST_CHANNEL14          (0b0000100000001110)
   #define DMA_REQUEST_CHANNEL15          (0b0000100000001111)
   #define DMA_REQUEST_CHANNEL16          (0b0000100000010000)
   #define DMA_REQUEST_CHANNEL17          (0b0000100000010001)
   #define DMA_REQUEST_CHANNEL18          (0b0000100000010010)
   #define DMA_REQUEST_CHANNEL19          (0b0000100000010011)
   #define DMA_REQUEST_CHANNEL20          (0b0000100000010100)
   #define DMA_REQUEST_CHANNEL21          (0b0000100000010101)
   #define DMA_REQUEST_CHANNEL22          (0b0000100000010110)
   #define DMA_REQUEST_CHANNEL23          (0b0000100000010111)
   #define DMA_REQUEST_CHANNEL24          (0b0000100000011000)
   #define DMA_REQUEST_CHANNEL25          (0b0000100000011001)
   #define DMA_REQUEST_CHANNEL26          (0b0000100000011010)
   #define DMA_REQUEST_CHANNEL27          (0b0000100000011011)
   #define DMA_REQUEST_CHANNEL28          (0b0000100000011100)
   #define DMA_REQUEST_CHANNEL29          (0b0000100000011101)
   #define DMA_REQUEST_CHANNEL30          (0b0000100000011110)
   #define DMA_REQUEST_CHANNEL31          (0b0000100000011111)
*/

#define DMA_DONE_CHANNEL0              (0b0000100001000000) //!< Event DMA Channel 0 Transfer done / acknowledge
#define DMA_DONE_CHANNEL1              (0b0000100001000001) //!< Event DMA Channel 1 Transfer done / acknowledge
#define DMA_DONE_CHANNEL2              (0b0000100001000010) //!< Event DMA Channel 2 Transfer done / acknowledge
#define DMA_DONE_CHANNEL3              (0b0000100001000011) //!< Event DMA Channel 3 Transfer done / acknowledge
#define DMA_DONE_CHANNEL4              (0b0000100001000100) //!< Event DMA Channel 4 Transfer done / acknowledge
#define DMA_DONE_CHANNEL5              (0b0000100001000101) //!< Event DMA Channel 5 Transfer done / acknowledge
#define DMA_DONE_CHANNEL6              (0b0000100001000110) //!< Event DMA Channel 6 Transfer done / acknowledge
#define DMA_DONE_CHANNEL7              (0b0000100001000111) //!< Event DMA Channel 7 Transfer done / acknowledge
/*
   #define DMA_DONE_CHANNEL8              (0b0000100001001000)
   #define DMA_DONE_CHANNEL9              (0b0000100001001001)
   #define DMA_DONE_CHANNEL10             (0b0000100001001010)
   #define DMA_DONE_CHANNEL11             (0b0000100001001011)
   #define DMA_DONE_CHANNEL12             (0b0000100001001100)
   #define DMA_DONE_CHANNEL13             (0b0000100001001101)
   #define DMA_DONE_CHANNEL14             (0b0000100001001110)
   #define DMA_DONE_CHANNEL15             (0b0000100001001111)
   #define DMA_DONE_CHANNEL16             (0b0000100001010000)
   #define DMA_DONE_CHANNEL17             (0b0000100001010001)
   #define DMA_DONE_CHANNEL18             (0b0000100001010010)
   #define DMA_DONE_CHANNEL19             (0b0000100001010011)
   #define DMA_DONE_CHANNEL20             (0b0000100001010100)
   #define DMA_DONE_CHANNEL21             (0b0000100001010101)
   #define DMA_DONE_CHANNEL22             (0b0000100001010110)
   #define DMA_DONE_CHANNEL23             (0b0000100001010111)
   #define DMA_DONE_CHANNEL24             (0b0000100001011000)
   #define DMA_DONE_CHANNEL25             (0b0000100001011001)
   #define DMA_DONE_CHANNEL26             (0b0000100001011010)
   #define DMA_DONE_CHANNEL27             (0b0000100001011011)
   #define DMA_DONE_CHANNEL28             (0b0000100001011100)
   #define DMA_DONE_CHANNEL29             (0b0000100001011101)
   #define DMA_DONE_CHANNEL30             (0b0000100001011110)
   #define DMA_DONE_CHANNEL31             (0b0000100001011111)
*/

#define ADC_ARM_ADC_SH0                (0b0000110010000001) //!< Event ADC Sample&Hold Stage #0 sampling analog input
#define ADC_ARM_ADC_SH1                (0b0000110010000010) //!< Event ADC Sample&Hold Stage #1 sampling analog input
#define ADC_ARM_ADC_SH2                (0b0000110010000100) //!< Event ADC Sample&Hold Stage Temperature Sensor sampling analog input
/*
   #define ADC_ARM_ADC_SH3                (0b0000110010001000)
   #define ADC_ARM_ADC_SH4                (0b0000110010010000)
   #define ADC_ARM_ADC_SH5                (0b0000110010100000)
*/

#define ADC_TRIGGER_ADC_SH0            (0b0000110011000001) //!< Event ADC Sample&Hold Stage #0 hold and convert
#define ADC_TRIGGER_ADC_SH1            (0b0000110011000010) //!< Event ADC Sample&Hold Stage #1 hold and convert
#define ADC_TRIGGER_ADC_SH2            (0b0000110011000100) //!< Event ADC Sample&Hold Temperature Sensor hold and convert
/*
   #define ADC_TRIGGER_ADC_SH3            (0b0000110011001000)
   #define ADC_TRIGGER_ADC_SH4            (0b0000110011010000)
   #define ADC_TRIGGER_ADC_SH5            (0b0000110011100000)
*/

#define SYS_TIM_INPUT_START_CH0        (0b0010110000000001) //!< Event System Timer channel #0 start acc. to timer channel config
#define SYS_TIM_INPUT_START_CH1        (0b0010110000000010) //!< Event System Timer channel #1 start acc. to timer channel config
#define SYS_TIM_INPUT_START_CH2        (0b0010110000000100) //!< Event System Timer channel #2 start acc. to timer channel config
#define SYS_TIM_INPUT_START_CH3        (0b0010110000001000) //!< Event System Timer channel #3 start acc. to timer channel config
#define SYS_TIM_INPUT_START_CH4        (0b0010110000010000) //!< Event System Timer channel #4 start acc. to timer channel config
#define SYS_TIM_INPUT_START_CH5        (0b0010110000100000) //!< Event System Timer channel #5 start acc. to timer channel config
#define SYS_TIM_INPUT_STOP_CH0         (0b0010110001000001) //!< Event System Timer channel #0 stop timer operation
#define SYS_TIM_INPUT_STOP_CH1         (0b0010110001000010) //!< Event System Timer channel #1 stop timer operation
#define SYS_TIM_INPUT_STOP_CH2         (0b0010110001000100) //!< Event System Timer channel #2 stop timer operation
#define SYS_TIM_INPUT_STOP_CH3         (0b0010110001001000) //!< Event System Timer channel #3 stop timer operation
#define SYS_TIM_INPUT_STOP_CH4         (0b0010110001010000) //!< Event System Timer channel #4 stop timer operation
#define SYS_TIM_INPUT_STOP_CH5         (0b0010110001100000) //!< Event System Timer channel #5 stop timer operation

#define HB_STOP                        (0b0011100000000000) //!< Event H-Bridge STOP operation (all switches off)
#define HB_FORWARD                     (0b0011100000000001) //!< Event H-Bridge active Direction  (direction HS1 on , LS1 off, HS2 off, LS2 on)
#define HB_BACKWARD                    (0b0011100000000010) //!< Event H-Bridge opposite Direction  (direction HS1 off , LS1 on, HS2 on, LS2 off)
#define HB_FREEWHEEL_LOW               (0b0011100000000011) //!< Event H-Bridge clamp Low operation (HS1 off, LS1 on, HS2 off, LS2 on)
#define HB_FREEWHEEL_HIGH              (0b0011100000000100) //!< Event H-Bridge clamp High operation (HS1 on, LS1 off, HS2 on, LS2 off)


#ifdef __cplusplus
}
#endif

/** @} */ /* End of group event block */


/** @} */ /* End of group Smack */

/** @} */ /* End of group Infineon */

#endif /* _EVENT_H_ */
