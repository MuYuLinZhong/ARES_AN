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
 * SmAcK memory definitions
 */

__ROM_BASE = 0x00000000;
__ROM_SIZE = 0x00004000;

__DPARAM_BASE = 0x00010000;
__DPARAM_SIZE = 0x00000400;
__APARAM_BASE = 0x00010400;
__APARAM_SIZE = 0x00000400;
__NVM_BASE    = 0x00010800;
__NVM_SIZE    = 0x0000E800;

__RAM_BASE  = 0x00020000;
__RAM_SIZE  = 0x00002000;
__RAM2_BASE = 0x20000000;
__RAM2_SIZE = 0x00002000;

__STACK_SIZE = 0x00000400;
__HEAP_SIZE  = 0x00000000;

/* The code space contains a section for code identification.
   The identification is and added to the .elf file through some
 * python elftools magic as a post-build step.
 */
section_version_size = 12;
__ROM_SIZE = __ROM_SIZE - section_version_size;
__NVM_SIZE = __NVM_SIZE - section_version_size;

/*
;-------------------- <<< end of configuration section >>> --------------------
*/

MEMORY
{
	ROM (rx)    : ORIGIN = __ROM_BASE,      LENGTH = __ROM_SIZE
	DPARAM (rx) : ORIGIN = __DPARAM_BASE,   LENGTH = __DPARAM_SIZE
	APARAM (rx) : ORIGIN = __APARAM_BASE,   LENGTH = __APARAM_SIZE
	NVM (rx)    : ORIGIN = __NVM_BASE,      LENGTH = __NVM_SIZE
	RAM   (rwx) : ORIGIN = __RAM_BASE,      LENGTH = __RAM_SIZE
	RAM2  (rwx) : ORIGIN = __RAM2_BASE,     LENGTH = __RAM2_SIZE
}
