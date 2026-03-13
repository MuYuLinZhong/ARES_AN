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
 * @file     version.h
 *
 * @brief    Definition of version scheme.
 *
 * @version  v1.0
 * @date     2020-05-20
 *
 * @note
 */

/*lint -save -e960 */

#ifndef _VERSION_H_
#define _VERSION_H_

/*
==============================================================================
   1. INCLUDE FILES
==============================================================================
*/

#include <stdint.h>            /* uint*_t */

/** @addtogroup Infineon
 * @{
 */

/** @addtogroup Smack
 * @{
 */


/** @addtogroup version
 * @{
 */

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @defgroup group_version_interface Interface of the version IDs
 * @ingroup group_interface_description
 *
 * For details about the version scheme refer to @ref pgSmack_Id.
 */

/*
==============================================================================
   2. DEFINITIONS
==============================================================================
*/

/**
 * @ingroup group_version_fw
 * Definition of the platform ID
 *
 * @note Each definition must not use more than 4 bit
 */
/*lint -e749 */
typedef enum
{
    DP2       = 0x2, //!< 0x2 denotes DP2 platform
    DP3       = 0x3, //!< 0x3 denotes DP3 platform
    DANDELION = 0x4  //!< 0x4 denotes Dandelion platform
} Platform_t;

/**
 * @ingroup group_version_fw
 * Definition of the platform extension ID
 *
 * @note Each definition must not use more than 4 bit
 */
typedef enum
{
    DPxA  = 0xA,   //!< 0xA denotes DPxA platform extension
    DPxB  = 0xB,   //!< 0xB denotes DPxB platform extension
    SMACK = 0xC    //!< 0xC denotes Smack platform extension
} Platform_Version_t;

/**
 * @ingroup group_version_fw
 * Definition of major, minor and step ID
 *
 * @{
 */
#define FW_VERSION_MAJOR  (0x4)         //!< Major version. Must start with 1 for tape out relevant releases.
#define FW_VERSION_MINOR  (0x0)         //!< Minor version. Must start with 0, when major version is increased.
#define FW_VERSION_STEP   (0x0000)      //!< Step version 16 bits. For internal releases.
/// @}


/**
 * @ingroup group_version_patch
 * Definition of major, minor, step and commit ID
 *
 * @{
 */
#define PATCH_VERSION_MAJOR  (0x00)         //!< Major version. 0 denotes 'undefined'
#define PATCH_VERSION_MINOR  (0x00)         //!< Minor version. Must start with 0, when major version is changed
#define PATCH_VERSION_STEP   (0x00)         //!< Step version. Must start with 0, when major version is changed
#define PATCH_VERSION_COMMIT (0x00000000)   //!< Commit ID. 0 denotes 'undefined'
/// @}
/*
==============================================================================
   3. TYPES
==============================================================================
*/
/**
 * @defgroup group_version_fw Type definition of FW version
 * @ingroup group_version_interface
 * @brief Type definitions for FW version scheme
 *
 * @{
 */
typedef struct
{
    uint8_t  platform;      //!< combines platform ID and platform ID extension
    uint8_t  version;       //!< combines ::FW_VERSION_MAJOR and ::FW_VERSION_MINOR
    uint16_t step;          //!< contains ::FW_VERSION_STEP
    uint8_t  commit_id[3];  //!< contains git first 3 bytes (6 hex digits) of git commit_id
    uint8_t  dirty;         //!< contains git dirty repo status
    uint32_t crc;           //!< contains code image crc
} Version_t;
/// @}

/**
 * @defgroup group_version_patch Type definition of Patch version
 * @ingroup group_version_interface
 * @brief Type definitions for patch version scheme
 *
 * @{
 */
typedef struct
{
    uint8_t  major;         //!< contains ::PATCH_VERSION_MAJOR
    uint8_t  minor;         //!< contains ::PATCH_VERSION_MINOR
    uint16_t step;          //!< contains ::PATCH_VERSION_STEP
    uint32_t commit_id;     //!< contains the commit ID
} Patch_Version_t;

/// @}


/*
==============================================================================
   4. EXPORTED DATA
==============================================================================
*/
extern const Version_t version;
extern Patch_Version_t patch_version;


/*
==============================================================================
   5. FUNCTION PROTOTYPES
==============================================================================
*/


/*
==============================================================================
   6. INLINE FUNCTIONS
==============================================================================
*/

#ifdef __cplusplus
}
#endif

/** @} */ /* End of group version */


/** @} */ /* End of group Smack */

/** @} */ /* End of group Infineon */

#endif /* _VERSION_H_ */
