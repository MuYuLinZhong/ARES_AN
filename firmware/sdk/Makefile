# ============================================================================
# Copyright (c) 2021 Infineon Technologies AG
#               All rights reserved.
#               www.infineon.com
# ============================================================================
#
# ============================================================================
# Redistribution and use of this software only permitted to the extent
# expressly agreed with Infineon Technologies AG.
# ============================================================================

###################################################################################################
# Variables
###################################################################################################
# @todo AP not used anywhere ..
# @todo AP why not use CURDIR? Quote from the manual:
# For your convenience, when GNU make starts (after it has processed any -C options)
# it sets the variable CURDIR to the pathname of the current working directory.
REPO_ROOT_DIR := $(abspath .)

# Firmware distribution package parameter for the 'dist' target
DIST_NAME := SMACK_FW_BUILD
DIST_LIST := $(REPO_ROOT_DIR)/tool_config/distribution/rtlsim_dist_file_list
BUILD_DIR := $(REPO_ROOT_DIR)/build

# Obtain a list of projects which build a rom image or a patch.
# Obviously, this only makes sense when this Makefile is run in REPO_ROOT_DIR ...
ROM_PROJ_DIRS :=
NVM_LIB_DIRS :=
NVM_PROJ_DIRS := smack_sl
PATCH_PROJ_DIRS :=$(wildcard  smack_patch*)
TEST_ROM_PROJ_DIRS := 
PRODUCTION_TEST_PROJ_DIR := smack_production_test

# Obtain a list of all projects
PROJ_DIRS := \
    $(ROM_PROJ_DIRS) \
	$(NVM_LIB_DIRS) \
	$(NVM_PROJ_DIRS) \
    $(TEST_ROM_PROJ_DIRS)

# a) include rules, targets, variables which are common for every Makefile in each project
# b) provide macros that are OS-dependant
include $(REPO_ROOT_DIR)/tools/intern/make/env.mk
# provide the rules and target(s) for all the tools that are needed when building: compiler, doxy, linter, ...
include $(REPO_ROOT_DIR)/tools/intern/make/tools.mk

###################################################################################################
# Targets
###################################################################################################

# Generate build-rules for each project directory
# @todo AP To me, this is massively misleading: BUILD_PROJ_DIRS is not a list of
# directories, but a list of targets and/or prerequisites.
# At least this is how it is used further below.
# @todo AP To me, naming a target 'build-smack_rom' is bad: It is self-explaining that
# we want to build sth, the point of a target name is 'what do we want to build?'
#
# At the end of day, these macros are
# a) local to this Makefile
# b) used both as targets and prerequisites in the rules below to hand over
# other (!) targets to Makefile in the projects
BUILD_PROJ_DIRS          = $(PROJ_DIRS:%=build-%)
PACKAGE_RTLSIM_PROJ      = $(PROJ_DIRS:%=package_rtlsim-%)
CLEAN_PROJ_DIRS          = $(PROJ_DIRS:%=clean-%)
CLEAN_PATCH_PROJ_DIRS    = $(PATCH_PROJ_DIRS:%=clean_patch-%)

.PHONY: help
help:
	$(V)$(ECHO) 'make all              Build all subtargets: '$(BUILD_PROJ_DIRS)'
	$(V)$(ECHO) 'make clean            Clean all subtargets: '$(CLEAN_PROJ_DIRS)'

# The prerequisite for building 'all' is a list of all projects where we need to look
# whether each one has an 'all' target.
all: $(BUILD_PROJ_DIRS)

# 1. we can't build any target without having the necessary build tools available
# @todo AP adding 'tools' is not ok in my opinion:
# a) invoking smack_rom/Makefile includes imagebuild.mk which includes tools.mk again
# b) imagebuild.mk should take care of the necessary tools, not the top-lvl Makefile
# 2. invoke Makefile in respective project, telling it to build 'all'
#
# what is $(@:build-%=%) doing?
# a) $@ is the current target, so one out 'build-smack_rom, build-smack_patch, ...
# b) :build-%=% is pattern-matching 'build-' in $@ and replacing it by ... nothing.
# => we go back to 'smack_rom', 'smack_patch' ... WTF?
# @todo AP To me, a much more readable way of doing this is to use a filter for PROJ_DIRS
$(BUILD_PROJ_DIRS): tools
	$(MAKE) -C $(@:build-%=%) all

# building all RTL simulation packages ('package_rtlsim') depends on the
# 'RTL simulation package' target in the makefile of each project.
package_rtlsim: $(PACKAGE_RTLSIM_PROJ)
$(PACKAGE_RTLSIM_PROJ): tools
	$(MAKE) -C $(@:package_rtlsim-%=%) package_rtlsim
    
clean: $(CLEAN_PROJ_DIRS)
$(CLEAN_PROJ_DIRS):
	@$(MAKE) -C $(@:clean-%=%) clean

clean_patches: $(CLEAN_PATCH_PROJ_DIRS)
$(CLEAN_PATCH_PROJ_DIRS):
	@$(MAKE) -C $(@:clean_patch-%=%) clean

.PHONY: subdirs $(PROJ_DIRS)
.PHONY: $(BUILD_PROJ_DIRS)
.PHONY: $(CLEAN_PROJ_DIRS)
.PHONY: all clean
