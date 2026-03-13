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

include $(REPO_ROOT_DIR)/tools/intern/make/env.mk
include $(REPO_ROOT_DIR)/tools/intern/make/tools.mk

###################################################################################################
# Variables
###################################################################################################

# Setting of names for build directory
# note: "image" dir name needed for RTL Simulations. Do not change!
IMAGE_BUILD_DIR := $(BUILD_DIR)/image
APARAM_IMAGE_BUILD_DIR := $(BUILD_DIR)/aparams
DPARAM_IMAGE_BUILD_DIR := $(BUILD_DIR)/dparams

# Setting of names for images
IMAGE_ROM_NAME := image_rom
IMAGE_NVM_NAME := image_nvm
# IMAGE_RAM_NAME may already be set by jlink makefile
ifndef IMAGE_RAM_NAME
IMAGE_RAM_NAME := image_ram
endif

# find all (!) .c source files in the given aparam source dir
APARAM_SOURCE_FILES := $(foreach dir, $(PROJECT_APARAM_SOURCE_DIRS:%/=%), $(wildcard  $(dir)/*.c) )

# find all (!) .c source files in the given aparam source dir
DPARAM_SOURCE_FILES := $(foreach dir, $(PROJECT_DPARAM_SOURCE_DIRS:%/=%), $(wildcard  $(dir)/*.c) )

# The linked ELF file to post-processed to calculate a CRC-32 checksum.
# This checksum is set in the corresponding target ELF file.
LINKED_IMAGE_ROM_FILE := $(IMAGE_BUILD_DIR)/$(IMAGE_ROM_NAME)_nocrc.elf
TARGET_IMAGE_ROM_FILE := $(IMAGE_BUILD_DIR)/$(IMAGE_ROM_NAME).elf
TARGET_IMAGE_ROM_HEX  := $(IMAGE_BUILD_DIR)/$(IMAGE_ROM_NAME).hex
TARGET_RTL_ROM_FILE   := $(IMAGE_BUILD_DIR)/$(IMAGE_ROM_NAME).rom

# The linked ELF file to post-processed to calculate a CRC-32 checksum.
# This checksum is set in the corresponding target ELF file.
LINKED_IMAGE_NVM_FILE := $(IMAGE_BUILD_DIR)/$(IMAGE_NVM_NAME)_nocrc.elf
TARGET_IMAGE_NVM_FILE := $(IMAGE_BUILD_DIR)/$(IMAGE_NVM_NAME).elf
TARGET_IMAGE_NVM_HEX  := $(IMAGE_BUILD_DIR)/$(IMAGE_NVM_NAME).hex
TARGET_RTL_NVM_FILE   := $(IMAGE_BUILD_DIR)/$(IMAGE_NVM_NAME).rom

# The linked ELF file to post-processed to calculate a CRC-32 checksum.
LINKED_IMAGE_RAM_FILE := $(IMAGE_BUILD_DIR)/$(IMAGE_RAM_NAME)_nocrc.elf
TARGET_IMAGE_RAM_FILE := $(IMAGE_BUILD_DIR)/$(IMAGE_RAM_NAME).elf
TARGET_IMAGE_RAM_HEX  := $(IMAGE_BUILD_DIR)/$(IMAGE_RAM_NAME).hex
TARGET_RTL_RAM_FILE   := $(IMAGE_BUILD_DIR)/$(IMAGE_RAM_NAME).ram

# find all (!) .elf files in the given aparam build dir
LINKED_APARAM_IMAGE_FILES = $(sort $(foreach file, $(notdir $(APARAM_SOURCE_FILES)), \
               $(APARAM_IMAGE_BUILD_DIR)/$(basename $(file)).elf))

APARAM_XML_FILES = $(sort $(foreach file, $(notdir $(APARAM_SOURCE_FILES)), \
               $(APARAM_IMAGE_BUILD_DIR)/$(basename $(file)).xml))

# find all (!) .elf files in the given dparam build dir
LINKED_DPARAM_IMAGE_FILES = $(sort $(foreach file, $(notdir $(DPARAM_SOURCE_FILES)), \
               $(DPARAM_IMAGE_BUILD_DIR)/$(basename $(file)).elf))

DPARAM_XML_FILES = $(sort $(foreach file, $(notdir $(DPARAM_SOURCE_FILES)), \
               $(DPARAM_IMAGE_BUILD_DIR)/$(basename $(file)).xml))


# Python script calculates the code identificaiton of the FW image and sets the CRC value in the
# 	'.version' section of the ELF file.
CODE_ID_CALCULATOR := $(SCRIPT_DIR)/code_id_calculator.py

###################################################################################################
# Targets
###################################################################################################

# we have 2(at least, for more, see also gcc_compile.mk) post-processing steps for 
# LINKED_IMAGE_ROM_FILE:
# - Python script calculates the ROM code CRC-32 and sets the CRC value in the
# 	'.version' section of the ELF file.
$(TARGET_IMAGE_ROM_FILE): $(LINKED_IMAGE_ROM_FILE) $(LINKED_APARAM_IMAGE_FILES) $(LINKED_DPARAM_IMAGE_FILES) | $(CODE_ID_CALCULATOR)
	@$(ECHO) calculating the code identification (commit_id, dirty, crc) of $<, including it into $@
	$(V)$(PYTHON) $(CODE_ID_CALCULATOR) 'ROM' $< $@

# we have 2(at least, for more, see also gcc_compile.mk) post-processing steps for 
# LINKED_IMAGE_NVM_FILE:
# - Python script calculates the NVM code CRC-32 and sets the CRC value in the
# 	'.version' section of the ELF file.
ifeq ($(SDK),0)	
# with Python
$(TARGET_IMAGE_NVM_FILE): $(LINKED_IMAGE_NVM_FILE) $(LINKED_APARAM_IMAGE_FILES) $(LINKED_DPARAM_IMAGE_FILES) | $(CODE_ID_CALCULATOR)
	@$(ECHO) calculating the code identification (commit_id, dirty, crc) of $<, including it into $@
	$(V)$(PYTHON) $(CODE_ID_CALCULATOR) 'NVM' $< $@
else
# no Python
$(TARGET_IMAGE_NVM_FILE): $(LINKED_IMAGE_NVM_FILE) $(LINKED_APARAM_IMAGE_FILES) $(LINKED_DPARAM_IMAGE_FILES)
	@$(ECHO) copying $< into $@
	$(V)$(COPY) $(subst /,\, $<) $(subst /,\, $@)
endif

# we have 2(at least, for more, see also gcc_compile.mk) post-processing steps for 
# LINKED_IMAGE_RAM_FILE:
# - Python script calculates the RAM code CRC-32 and sets the CRC value in the
# 	'.version' section of the ELF file.
$(TARGET_IMAGE_RAM_FILE): $(LINKED_IMAGE_RAM_FILE) $(LINKED_APARAM_IMAGE_FILES) $(LINKED_DPARAM_IMAGE_FILES) | $(CODE_ID_CALCULATOR)
	@$(ECHO) calculating the code identification (commit_id, dirty, crc) of $<, including it into $@
	$(V)$(PYTHON) $(CODE_ID_CALCULATOR) 'RAM' $< $@

	
# For the aparam stuff:
# What is left to do (but will never be done, we move to scons first ... :-):
# - hard-coded UC, xml filename
# - hopping through a bat file, we need to invoke Python3 which we borrow from Inicio :-(

image_rom: $(APARAM_XML_FILES) $(DPARAM_XML_FILES)
image_nvm: $(APARAM_XML_FILES) $(DPARAM_XML_FILES)
#image_ram: $(APARAM_XML_FILES) $(DPARAM_XML_FILES)

# previous call of wrapper batch file: @$(SCRIPT_DIR)/create_structure_from_elf.bat $*.elf $*.xml $(APARAM_INST_NAME) $(APARAM_TYPE_NAME) $(TARGET_IMAGE_ROM_FILE)
# batch file content as makefile line: $(PYTHON) $(SCRIPT_DIR)/cli_xml_creator.py --verbose --initial_values -i $*.elf -o $*.xml -sn $(APARAM_INST_NAME) -st $(APARAM_TYPE_NAME)
# New script calls without the wrapper batch file that calls external Python instance of Inicio:
$(APARAM_XML_FILES): %.xml: %.elf $(TARGET_IMAGE_ROM_FILE)
	@$(ECHO) extracting aparams from $*.elf and create XML
	$(V)$(PYTHON) $(SCRIPT_DIR)/cli_xml_creator.py --initial_values -i $*.elf -o $*.xml -sn $(APARAM_INST_NAME) -st $(APARAM_TYPE_NAME)

$(DPARAM_XML_FILES): %.xml: %.elf %.hex $(TARGET_IMAGE_ROM_FILE)
	@$(ECHO) extracting dparams from $*.elf and create XML
	$(V)$(PYTHON) $(SCRIPT_DIR)/cli_xml_creator.py --initial_values -i $*.elf -o $*.xml -sn $(DPARAM_INST_NAME) -st $(DPARAM_TYPE_NAME)

###################################################################################################
# Display the help menue
###################################################################################################

# If this is the first target (not overruled in the Makefile) and it will be called when no
# argument is given to GNUmake. The target specific help relies on the general help target and
# extends it by the target specific parts.
help: rombuild_help
.PHONY: rombuild_help
rombuild_help:
	@$(ECHO) 'make all              Build all targets'

###################################################################################################
# Framework Includes
###################################################################################################

#include $(REPO_ROOT_DIR)/tools/intern/make/linting.mk
#include $(REPO_ROOT_DIR)/tools/intern/make/astyle.mk
include $(REPO_ROOT_DIR)/tools/intern/make/gcc_compile.mk
