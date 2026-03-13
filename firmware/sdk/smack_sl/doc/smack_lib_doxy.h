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
 * @file    smack_lib_doxy.h
 * @brief   Top-level container for (DOXYGEN-style) SMACKLib FW documentation.
 */

#ifndef SMACKLIB_DOXY_H
#define SMACKLIB_DOXY_H

/**
@page pgSmack_FW_Modules Smack Library
 
The Smack library (SmackLib) is made up of the following FW modules:

/* - subpage pgSmack_Scheduler */
/* - subpage pgLib_StructExc */
/* - subpage pgSmack_Otp */
/* - subpage pgSmack_Patch */
/* - subpage pgSmack_CmdHandler */
/* - subpage pgSmack_Uart */
/* - subpage pgLib_Fsm */
/* - subpage pgSmack_Psm */
/* - subpage pgSmack_Aparam */
- @subpage pgSmack_Dparam
/* - subpage pgSmack_boot */
/* - subpage pgSmack_spi_eeprom */
/* - subpage pgSmack_spi_driver */
/* - subpage memory_pool */
/* - subpage pgSmack_crc16_ccitt */
/* - subpage pgSmack_Prod_Test */

Each module comes with documentation which is basically structured into at least 2 chapters. One chapter is on the @e architecture of the
FW module and a second chapter on noteworthy @e implementation details. Insofar, the structure of this document
is recursive - @e Architecture and @e Implementation are recurring headlines throughout this documentation, available
for each FW module.

Following the main conceptual decisions for the SMACKLib are discussed. If applicable, to the documentation
of the respective FW modules are referred.

<b>Ownership and Responsibility</b>\n
The SmackLib is partly available as a (linkable) library and partly available as source code. This enables clear ownership
and responsibilities within the FW team. Access to the complete SmackLib source code is limited to the responsible owners.

<b>Hierarchy</b>\n
The SmackLib is not implemented with additional levels of @e hierarchy or @e layers in mind. Instead, its @e hierarchy is rather flat:
- SmackLib itself does not qualify as a @e layer, it serves as a @e library.
- There is a @ref sctSmack_Otp_Hal for accessing peripheral registers. The HAL is generated code from the ECB data base.
- Pure infrastructure peripherals like OTP or UART are wrapped into drivers, but power peripherals like QR, LLC and system timer needs
to prove that they deserve their own driver. Their HW design is done in a way that handling by any FW is supposed to be simple,
making an extra driver layer at least questionable.
- There is @b no extra layer for something like a 'low level driver' or alike. Instead, everybody uses the HAL when accessing peripherals.
'Drivers' which are simply wrapping a bunch of HAL accesses are not allowed.
- Every developer is required to group HAL access by peripherals in the source code. There is no problem with accessing different
peripherals through the HAL in one function.
- The FW team periodically reviews the code to assess whether repetitive code deserves to be refactored into a driver.

<b>Memory Management</b>\n
Memory management is done partially at run-time. During start-up, applicable FW modules obtain a memory block for their run-time data.
Such concept requires that FW modules carefully structure their respective data into state and configuration data.
More details are found in the section @ref memory_pool.

<b>FW start-up procedure</b>\n
The FW start-up procedure is separated into 2 phases:
-# The boot code is executed after PoR, after a FW-triggered reboot and when waking up from selected PSM.
 The boot code cannot be patched. It is owned by SMACKLib, more details are found in the section @ref pgSMACK_boot.
-# The application start is executed after the boot code is complete and when waking up from selected PSM.
 The application start may be controlled by an applicable OTP record (e.g. the application parameters) or by
 some data in RAM, prepared by the PSM entry code. The application start is owned by the SMACKB application.

<b>Exception Handling</b>\n
The exception handling module of the SMACKLib owns all exception handling functionality:

- A FW functionality that needs to escalate an exception to a higher FW entity is doing this by C++/object-oriented ::TRY ::CATCH ::THROW statements.
  This avoids tedious and complicate back-propagation of return values through the current call stack.
- An exception handling hierarchy is implemented. SMACKLib provides separate default exception handlers for thread-level execution
  (aka background) and ISR-level execution (aka foreground and interrupts). Such default exception handlers implement protection against catastrophic
  failures (that is, switching off all gate drivers). The default exception handlers can and should be extended (!) by application code.
- The SMACKLib uses ARM M0 HW registers to obtain status information on the current execution level to implement its exception handling.
- A ::THROW block elevates any exception to any of the previously mentioned default exception handlers.
- The application that uses SMACKLib functions either
  -# relies on default exception handling (which would be truly bad and requires approval of the FW architect or technical lead)
  -# or has provided an extension to the default exception handler. Such extension handles the exception.
  -# or has provided its own ::TRY ::CATCH ::TRY_END block
- The stack memory for storing the CPU context at a ::TRY statement and the run-time effort of unwinding and restoring the context through a
  ::THROW statement is carefully considered and optimized. For corner cases, the FW developer may revert to other means of exception handling.
  This requires approval of the FW architect or technical lead.

More details are found in the section @ref pgLib_StructExc.


*/


#endif /* SMACKLIB_DOXY_H */
