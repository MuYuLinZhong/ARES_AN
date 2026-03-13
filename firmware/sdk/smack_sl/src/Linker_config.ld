/* ============================================================================
** Copyright (c) 2021 Infineon Technologies AG
**               All rights reserved.
**               www.infineon.com
** ============================================================================
*
*/

/******************************************************************************
 * @file     gcc_arm.ld
 * @brief    GNU Linker Script for Cortex-M based device
 * @version  V2.0.0
 * @date     21. May 2019
 ******************************************************************************/
/*
 * Copyright (c) 2009-2019 Arm Limited. All rights reserved.
 *
 * SPDX-License-Identifier: Apache-2.0
 *
 * Licensed under the Apache License, Version 2.0 (the License); you may
 * not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 * www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an AS IS BASIS, WITHOUT
 * WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

OUTPUT_FORMAT("elf32-littlearm")
OUTPUT_ARCH(arm)


INCLUDE smack_memory.ld

section_version_base = __NVM_BASE + __NVM_SIZE;
MEMORY
{
	/* holds version and code identification for NVM application firmware*/
	version (r) : ORIGIN = section_version_base, LENGTH = section_version_size
}


/* Linker script to place sections and symbol values. Should be used together
 * with other linker script that defines memory regions ROM and RAM.
 * It references following symbols, which must be defined in code:
 *   Reset_Handler : Entry of reset handler
 *
 * It defines following symbols, which code can use without definition:
 *   __exidx_start
 *   __exidx_end
 *   __nvm_copy_table_start__
 *   __nvm_copy_table_end__
 *   __nvm_zero_table_start__
 *   __nvm_zero_table_end__
 *   __etext
 *   __data_start__
 *   __preinit_array_start
 *   __preinit_array_end
 *   __init_array_start
 *   __init_array_end
 *   __fini_array_start
 *   __fini_array_end
 *   __data_end__
 *   __bss_start__
 *   __bss_end__
 *   __end__
 *   end
 *   __HeapLimit
 *   __StackLimit
 *   __StackTop
 *   __stack
 *
 * It imports the following symbols from ROM:
 *   __romcode_ram_end_
 *   __romcode_ram2_end_
 * The linker will access further symbols, e.g.:
 *   rom_func_table
 */
ENTRY(NVM_Reset_Handler)


SECTIONS
{
	/* ------------------------------------------------------------------------ */

	/* Create a section that holds the FW version and code identification.
	 */
	.version :
	{
		/* make sure linker is not throwing it away, post-build step needs it */
		/* @todo name changes when file name changes ... nice.*/
		KEEP(*(.rodata.version))
	} > version

    /* The SMACK Romcode reserves some space in the NVM for device and application parameters
       named DPARAMs and APARAMs
       The DPARAMs will be filled by ATE testing and the APARAMs will be filled by NVM flashing
       of Smack application code by customers
       NVM pages  0- 7 are reserved for the DPARAM data structure,
       NVM pages  8-11 are reserved for configuration APARAMs,
       NVM pages 12-13 are reserved for secret APARAMs, and
       NVM pages 14-15 are reserved for other APARAMs
       Note: this leaves 464 pages for NVM firmware 
     */

	.dparam (NOLOAD):
	{
		. = ORIGIN(DPARAM);
		__nvm_dparam_section_start__ = .;
		KEEP(*(.nvm.DPARAMS))
		__nvm_dparam_section_end__ = .;
	} > DPARAM

	.aparam :
	{
		__nvm_aparam_section_start__ = .;
		KEEP(*(.nvm.APARAMS))
		__nvm_aparam_section_end__ = .;
	} > APARAM

	.text :
	{
		__NVM_FIRMWARE_START = .;
		
		KEEP(*(.vectors))
		*(.text*)

		KEEP(*(.init))
		KEEP(*(.fini))

		/* .ctors */
		*crtbegin.o(.ctors)
		*crtbegin?.o(.ctors)
		*(EXCLUDE_FILE(*crtend?.o *crtend.o) .ctors)
		*(SORT(.ctors.*))
		*(.ctors)

		/* .dtors */
		*crtbegin.o(.dtors)
		*crtbegin?.o(.dtors)
		*(EXCLUDE_FILE(*crtend?.o *crtend.o) .dtors)
		*(SORT(.dtors.*))
		*(.dtors)

		*(.rodata*)

		KEEP(*(.eh_frame*))
	} > NVM

	/* SG veneers:
	   All SG veneers are placed in the special output section .gnu.sgstubs. Its start address
	   must be set, either with the command line option ‘--section-start’ or in a linker script,
	   to indicate where to place these veneers in memory.
	 */
	.gnu.sgstubs :
	{
		. = ALIGN(32);
	} > NVM

	.ARM.extab :
	{
		*(.ARM.extab* .gnu.linkonce.armextab.*)
	} > NVM

	__exidx_start = .;
	.ARM.exidx :
	{
		*(.ARM.exidx* .gnu.linkonce.armexidx.*)
	} > NVM
	__exidx_end = .;

	.copy.table :
	{
		. = ALIGN(4);
		__nvm_copy_table_start__ = .;
		LONG (__etext)
		LONG (__data_start__)
		LONG ((__data_end__ - __data_start__) / 4)
    /** Add each additional data section here */
    /*
		LONG (__etext2)
		LONG (__data2_start__)
		LONG ((__data2_end__ - __data2_start__) / 4)
	*/
		__nvm_copy_table_end__ = .;
	} > NVM

	.zero.table :
	{
		. = ALIGN(4);
		__nvm_zero_table_start__ = .;
		LONG (__bss_start__)
		LONG ((__bss_end__ - __bss_start__) / 4) 
	/** Add each additional bss section here */
	/* 
		LONG (__bss2_start__)
		LONG ((__bss2_end__ - __bss2_start__) / 4) 
	*/
		__nvm_zero_table_end__ = .;
	} > NVM

	/* Location counter can end up 2byte aligned with narrow Thumb code but
	   __etext is assumed by startup code to be the LMA of a section in RAM
	   which must be 4byte aligned */
	__etext = ALIGN (4);

	.data_romcode :
	{
		. = ALIGN(4);
		__data_romcode_start__ = .;
		. = . + (__romcode_ram_end_ - __RAM_BASE);
		. = ALIGN(4);
		/* All data end */
		__data_romcode_end__ = .;
	} > RAM
    
	.data : AT (__etext)
	{
		. = ALIGN(4);
		__data_start__ = .;
		*(vtable)
		*(.data)
		*(.data.*)
        
		. = ALIGN(4);
		/* preinit data */
		PROVIDE_HIDDEN (__preinit_array_start = .);
		KEEP(*(.preinit_array))
		PROVIDE_HIDDEN (__preinit_array_end = .);

		. = ALIGN(4);
		/* init data */
		PROVIDE_HIDDEN (__init_array_start = .);
		KEEP(*(SORT(.init_array.*)))
		KEEP(*(.init_array))
		PROVIDE_HIDDEN (__init_array_end = .);


		. = ALIGN(4);
		/* finit data */
		PROVIDE_HIDDEN (__fini_array_start = .);
		KEEP(*(SORT(.fini_array.*)))
		KEEP(*(.fini_array))
		PROVIDE_HIDDEN (__fini_array_end = .);

		KEEP(*(.jcr*))
		. = ALIGN(4);
		/* All data end */
		__data_end__ = .;

	} > RAM

	/** 
	  * Secondary data section, optional 
	  * 
	  * Remember to add each additional data section
	  * to the .copy.table above to asure proper
	  * initialization during startup.
	  */
	/*
	__etext2 = ALIGN (4);

	.data2 : AT (__etext2)
	{
		. = ALIGN(4);
		__data2_start__ = .;
		*(.data2)
		*(.data2.*)
		. = ALIGN(4);
		__data2_end__ = .;

	} > RAM 
	*/

	/* The SMACK DMA requires placing the channel descriptor block at a 1024 Byte
       aligned address with a size of 512 Bytes. We place it as last RAM memory
       section to minimize the amount of wasted memory. 
       Background is the following consideration:
       - Dandelion as a platform supports up to 10 (16 due to alignment restrictions) channels, 
         with each 16 bytes primary and 16 bytes alternate descriptors.
         This yields a descriptor size of 16x16x2 = 512 bytes.
         Since 8k of RAM are supported we need additional 3 address bits
         to specify the base address (CORE_SCU_DMA_CFG.DDBA).
       - Smack supports 10 DMA channels, only, 
         with each 16 bytes primary and 16 bytes alternate descriptors.
         This yields a descriptor size of 10x16x2 = 320 bytes.
         Still, the alignment of the DMA descriptor must use the  
         Dandelion platform alignment of 512 bytes.
         Note: this leaves 192 bytes unused, 96 above DMA descriptors, and 96 above alternate DMA descriptors!
       */
	/* Only the remaining RAM2 space is therefore available for NVM located firmware */
	.ram2_dma  (NOLOAD) :
	{
		. = ALIGN (4);
		__ram2_dma_section_start__ = .;
		. = . + (__romcode_ram2_end_ - __RAM2_BASE);
		__ram2_dma_section_end__ = .;
	} > RAM2

	.ram2  (NOLOAD) :
	{
		. = ALIGN (4);
		*(.ram2)
		*(.ram2.*);		
		__ram2_section_end__ = .;
	} > RAM2

	.bss :
	{
		. = ALIGN(4);
		__bss_start__ = .;
		*(.bss)
		*(.bss.*)
		*(COMMON)
		. = ALIGN(4);
		__bss_end__ = .;
	} > RAM

	/**
	 * Secondary bss section, optional 
	 *
	 * Remember to add each additional bss section
	 * to the .zero.table above to asure proper
	 * initialization during startup.
	 */
	/*
	.bss2 :
	{
		. = ALIGN(4);
		__bss2_start__ = .;
		*(.bss2)
		*(.bss2.*)
		*(COMMON)
		. = ALIGN(4);
		__bss2_end__ = .;
	} > RAM
	*/

	.noinit (NOLOAD) :
	{
		. = ALIGN(4);
		__noinit_start__ = .;
		*(.noinit)
		*(.noinit.*)
		. = ALIGN(4);
		__noinit_end__ = .;
	} > RAM

	.heap :
	{
		. = ALIGN(4);
		__end__ = .;
		PROVIDE(end = .);
		. = . + __HEAP_SIZE;
		. = ALIGN(4);
		__HeapLimit = .;
	} > RAM
    	
	.stack :
	{
		. = ORIGIN(RAM) + LENGTH(RAM) - __STACK_SIZE;
		. = ALIGN(4);
		__StackLimit = .;
		. = . + __STACK_SIZE;
		. = ALIGN(4);
		__StackTop = .;
	} > RAM
	PROVIDE(__stack = __StackTop);

	/* Check if data + heap + stack exceeds RAM limit */
	ASSERT(__StackLimit >= __HeapLimit, "region RAM overflowed with stack")

	/* ------------------------------------------------------------------------ */
	/* Code Space Padding
	 * The GNU linker seems to have problems with filling the unused code space area with
	 * padding Bytes. The following section starts behind the '.code_text' section
	 * and the attached '.data' load section, and it ends before
	 * the '.version' section. Writing a single pad Byte at the end of the
	 * section trigger the padding fill operation. */
	pad_start = __etext + SIZEOF (.data);
	pad_size = section_version_base - pad_start - 1;
	.text.pad2 pad_start :
	{
		. = . + pad_size;
		BYTE(0xff);
		/* we fill the rest of the code section with 0xffff:
		This resembles an erased NVM. */
	} > NVM = 0xffff

	/* ------------------------------------------------------------------------ */ 		

}
