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
 * @file    smack_main_doxy.h
 * @brief   Top-level container for (DOXYGEN-style) SMACK FW documentation.
 */

#ifndef SMACK_MAIN_DOXY_H
#define SMACK_MAIN_DOXY_H

/**
@mainpage About this Document

@section about-scope-purpose Scope and Purpose

This manual describes both the architecture and the corresponding implementation of the built-in ROM firmware of SMACK.
It is about the common FW part of the SMACK family of devices (the SMACK library) @b and the product-specific part
of the SMACK device firmware (the SMACK application). It also covers the eco system around the firmware, e.g. the build flow,
the design rules and the way the source code is tested prior to any system-level integration test.

The manual also explains architectural decisions, constraints and the implementation and goes down as far as to function level.
That is, the firmware provides its functionality through APIs and such APIs are the common subject of discussion and refinement
between the firmware team (which implements the functionality behind the APIs) and the user groups (which eventually may use the APIs
to build products @b after SMACK is out and proven).

@section about-intended-audience Intended Audience

This document is intended
- for engineers who need an API-level of understanding of the firmware and
- for the firmware team itself, requiring utmost details.

This document comes in 2 flavors: One is a single document in `pdf` format distributed as the <em> Architecture and
Implementation Manual </em> and the other is a (huge) set of `html` files. Both flavors are built from the exact same source code
database.

The <b>'pdf' version</b> is restricted in content to limit its size and to allow actual reading. It can be (auto-)generated at any given time.
Therefore, the `pdf` version is mostly generated for the convenience of an audience that needs API-level of understanding only.

The <b>'html' version</b> is the complete source code documentation intended to be used by the firmware team only. It is intended to be used
with a browser.

Both flavors are subject to frequent changes.
This makes sure that this documentation is tightly locked to the content (i.e. the code) it actually documents and avoids the
usual inconsistencies between source code and its documentation in case both are maintained in @e separate data bases.
Moreover, this procedure allows quick, convenient and simple update.

*/

/* The hierarchy and order of all pages and subpages.
 * Note that smack_main_doxy.h MUST be processed first by doxygen to make sure the
 * hierarchy is adhered to. See also MakefileDox.mk.
 *
 */
/**
 * @page pgSmack_Intro                Introduction
 * @page pgSmack_Rules                Rules
 * @page pgSmack_FW_Modules           SMACK Library
 * @page pgSmack_application          Application
 * @page pgSmack_Tooling              Tooling
 * @page memory_section             Linkage
 * @page pgSmack_FW_Infrastructure    Build Infrastructure
 * @page pgSmack_unittest             Unit Test
 * @page pgSmack_FW_Test_Automation   Test Automation
 * @page pgSmack_Scrum                Scrum
 * @page pgSmack_Ref                  References
 /// @cond
 * @page CMSIS_MISRA_Exceptions     MISRA-C:2004 Compliance Exceptions
 /// @endcond
 */

/*
 * Setup headings for a better grouping of the module description.
 */
/**
 * @defgroup group_cmsis_description CMSIS
 * @defgroup group_interface_description Interfaces
 * @defgroup group_utest_description Unit Tests
 *
 */
#endif /* SMACK_MAIN_DOXY_H */
