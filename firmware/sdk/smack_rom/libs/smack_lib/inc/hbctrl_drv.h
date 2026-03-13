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
 * @file     hbctrl_drv.h
 *
 * @brief    Driver for the half bridge control block.
 *
 * @version  v1.0
 * @date     2020-05-20
 *
 * @note
 */

/*lint -save -e960 */

#ifndef _HBCTRL_DRV_H_
#define _HBCTRL_DRV_H_


/** @addtogroup Infineon
 * @{
 */

/** @addtogroup Smack
 * @{
 */


/** @addtogroup hbctrl
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief HB status types
 */
typedef enum status_type_e
{
    clamp_stat      , // clamping status
    switch_stat       // switch status
} status_type_t;

/**
 * @brief HB configuration/ tuning parameter structure
 */
typedef struct hb_config_struct
{
    bool slopetrtfx10;    //!< if true the slope rise and fall times given in slopetrtf are multiplied by 10
    uint32_t slopetrtf;   //!< sets the value of the slope rise and fall times, range 0...7
    bool slopeext;        //!< switches slope control from gate voltage of low side switch to load current
    bool slope_en;        //!< if true enables slope control for the low side switches of the H-bridge
    uint32_t ccset;       //!< configures internal compensation capacitors, range 0...3. If slopeext is set, use lowest value that suppresses ringing with a given load, else set to 0.
    bool brake_en;        //!< if true activate ACL as long as output voltage is pulled below GND instead of observing direction of load current flow
    bool acl_en;          //!< if true it enables automatic clamping (ACL) of an inductive load
    uint32_t acl_delay;   //!< sets the value of acl_delay, range 0...7. Specifies the delay until the ACL control circuit shortly releases clamping to see if clamping is still required.
} hb_config_struct_t;



/**
 * @brief  This functions returns back status information of the H-Bridge. The status of the four switches is reported or the clamping status is returned
 * @param stat_req the status type to be reported, it can be clamp_stat --> clamping status will be reported
 *        switch_stat --> the status of the 4 gate enable signals of the H-Bridge transistors is reported
 * @return unsigned 32-bit integer
 *         in case switch_stat is parameter, then 4LSB's report the switch state Bit(3): HS1, Bit(2) LS1, Bit(1) LS2, Bit(0) HS2
 *         in case clamp_stat is parameter, then the 2LSB reflect the clamping status
 *         The other bits are 0x0
 */
extern uint32_t get_hb_stat(status_type_t stat_req);

/**
 * @brief This functions switches on/ off the four transistors of the H-Bridge by direct SW access. It sets the HB control mode to direct CPU access. Event Bus is ignored.
 * @param hs1_set --> if true, then HS1 is on, else off
 * @param ls1_set --> if true, then LS1 is on, else off
 * @param hs2_set --> if true, then HS2 is on, else off
 * @param ls2_set --> if true, then LS2 is on, else off
 */
extern void     set_hb_switch(bool hs1_set, bool ls1_set, bool hs2_set, bool ls2_set);

/**
 * @brief This function enables the control of the H-Bridge via the event bus. Direct CPU access is not possible.
 * @param control_switches_by_eventbus, if true, then control by eventbus, else control by direct CPU access
 */
extern void     set_hb_eventctrl(bool control_switches_by_eventbus);

/**
 * @brief This function sets configuration parameters of the H-Bridge. The parameters are put into a struct. The struct content is processed by the function and applied to H-Bridge
 * @param hb_config
 */
extern void     set_hb_config(const hb_config_struct_t* hb_config);

/**
 * @brief This function controls the HB transistors via the eventbus. It sets the HB control unit into event controlled mode and issues events to the event bus.
 *        The HB is receiving these events and processing them. It is recommended to use the eventcodes below.
 * @param event a 32-bit unsigned integer, which is issued as event code to the event bus.
 *              look into event.h for HB defined HB related event constants (codes)
 *              HB_STOP                      --> HS1 off, LS1 off, HS2 off, LS2 off
 *              HB_FORWARD                   --> HS1 on , LS1 off, HS2 off, LS2 on
 *              HB_BACKWARD                  --> HS1 off, LS1 on , HS2 on , LS2 off
 *              HB_FREEWHEEL_LOW             --> HS1 off, LS1 on , HS2 off, LS2 on
 *              HB_FREEWHEEL_HIGH            --> HS1 on , LS1 off, HS2 on,  LS2 off
 */
extern void     set_hb_event(uint32_t event);

/**
 * @brief This function shorts VDDHB to GND. DON'T USE IN FINAL APPLICATION. ONLY for TEST/ANALYSIS PURPOSES:
 */
extern void discharge_CA(void);

#ifdef __cplusplus
}
#endif

/** @} */ /* End of group hbctrl block */


/** @} */ /* End of group Smack */

/** @} */ /* End of group Infineon */

#endif /* _HBCTRL_DRV_H_ */
