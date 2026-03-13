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

# check imported variables
# collection of directories where to find source code
#$(info PROJECT_SOURCE_DIRS: $(PROJECT_SOURCE_DIRS))
# project specific root directory (e.g. /smack_rom)
# does not include tool_config directory
# place where to fine main.c
#$(info PROJECT_ROOT_DIR: $(PROJECT_ROOT_DIR))

# Name of the build project directory.
# This information will be included in the documentation.
PROJECT_NAME := $(notdir $(realpath $(PROJECT_ROOT_DIR)))

# Path where to find the doxygen configuration file and other layout files.
# We need to specify separately because the path is not part of the source file pathes.
DOXYGEN_CONFIG_DIR := $(REPO_ROOT_DIR)/tool_config/doxygen

# Path where to find the doxygen.exe tool.
# Mandatory for executing the tool.
DOXYGEN_TOOL_DIR := $(REPO_ROOT_DIR)/tools/extern/Doxygen

# DOCGENERATOR_SOURCE_DIRS is input for the doxygen tool to search for any kind of
# doxygen documentation and figures.
# Note: The Doxygen tool will scan all these directories recursively!
# 1. PROJECT_ROOT_DIR is populated from the makefile 'above' this one: The so-called
# project makefile which includes this one in case the project wants/needs to support 'doc' targets
# 2. smack_main_doxy.h must be included first because it defines the page and subpage hierarchy/order.
# If doxygen processes it too late, the hierarchy/order cannot be adjusted ... :-(
DOCGENERATOR_SOURCE_DIRS := \
	$(DOCGENERATOR_EXTERNAL_SOURCE_DIRS) \
	$(DOXYGEN_CONFIG_DIR) \
	$(PROJECT_ROOT_DIR)

#$(info DOCGENERATOR_SOURCE_DIRS: $(DOCGENERATOR_SOURCE_DIRS))

# =======================================================================
# Define the document generator
# =======================================================================

# Input =================================================================
# main config file holding all input parameters for the doxygen tool
DOCGENERATOR_CONFIG_FILE := $(DOXYGEN_CONFIG_DIR)/DoxyFile

# Plausi check: only one doxygen config file is allowed
ifneq ($(words $(DOCGENERATOR_CONFIG_FILE)), 1)
    $(error Too many layout files found ($(DOCGENERATOR_CONFIG_FILE)).)
endif

# layout file to configure appearance of pages
DOCGENERATOR_DITA_LAYOUT_FILE := $(DOXYGEN_CONFIG_DIR)/Layout.xml

# search for optional layout file
DOCGENERATOR_LAYOUT_FILE := $(strip $(foreach dir,$(PROJECT_SOURCE_DIRS:%/=%),$(wildcard $(dir)/DoxygenLayout.xml)))

# info file for specifying documnet specific variables (e.g. title, doc type, etc.)
DOCGENERATOR_DOCINFO_FILE := $(DOXYGEN_CONFIG_DIR)/docinfo.txt

# PlantUML include path
PLANTUML_INCLUDE_PATH := $(REPO_ROOT_DIR)/tool_config/plantuml

# Dependencies ==========================================================
# DOCGENERATOR_FILES is used to specify the dependencies.
# If one of these files is changed, the documentation will be generated again.
DOCGENERATOR_FILES += $(DOCGENERATOR_CONFIG_FILE) \
                      $(DOCGENERATOR_DITA_LAYOUT_FILE) \
                      $(DOCGENERATOR_LAYOUT_FILE) \
                      $(DOCGENERATOR_DOCINFO_FILE)

## Obtain a list of all files from those directories that hold source files and
## use this list as a dependency for the document build.
## Note: subdirectories are not considered
#DOCGENERATOR_FILES += $(foreach dir, $(DOCGENERATOR_SOURCE_DIRS:%/=%), $(wildcard $(dir)/*.c $(dir)/*.h $(dir)/*.S))
#DOCGENERATOR_FILES += $(foreach dir, $(PROJECT_SOURCE_DIRS:%/=%), $(wildcard $(dir)/*.c $(dir)/*.h $(dir)/*.S))
#DOCGENERATOR_FILES += $(foreach dir, $(PROJECT_HEADER_DIRS:%/=%), $(wildcard $(dir)/*.c $(dir)/*.h $(dir)/*.S))
#
## Doxygen considers all files under PROJECT_ROOT_DIR (recursively, including test folder), but
## PROJECT_SOURCE_DIRS and PROJECT_HEADER_DIRS do not contain the test folder.
## Therefore define it here to complete the dependencies.
#PROJECT_TEST_DIRS := \
#    $(PROJECT_ROOT_DIR)/libs/smack_lib/test/ \
#    $(PROJECT_ROOT_DIR)/libs/smack_lib/test/support
#
#DOCGENERATOR_FILES += $(foreach dir, $(PROJECT_TEST_DIRS:%/=%), $(wildcard $(dir)/*.c $(dir)/*.h $(dir)/*.S))

#$(info DOCGENERATOR_FILES: $(DOCGENERATOR_FILES))

# Output ================================================================
# Output directory for the documentation
DOC_BUILD_DIR := $(BUILD_DIR)/doxygen
DOC_OUTPUT_LOG_FILE := $(DOC_BUILD_DIR)/output_log.txt

# =======================================================================
# Generate the documentation
# =======================================================================

# Variables =============================================================

# The exotic command below essentially pipes the DoxyFile to the cmdline for configuring
# doxygen (see doxygen FAQ). Furthermore, "-" tells doxygen to read its configuration from the
# cmdline (instead of a config file). This way selected parts of the configuration can be
# overwritten.
# Note: Instead of the prerequisite DOCGENERATOR_FILES, DOCGENERATOR_SOURCE_DIRS is used for INPUT
# since otherwise the cmdline for doxygen quickly gets too long.
# To increase speed, we generate html only.
# - EXCLUDE ..: exclude some files which are doubled up between lib and app.
DOCGENERATOR = \
( type $(subst /,\,$(DOCGENERATOR_CONFIG_FILE)) \
	& echo INPUT=$(DOCGENERATOR_SOURCE_DIRS) \
	& echo IMAGE_PATH=$(DOCGENERATOR_SOURCE_DIRS) \
	& echo OUTPUT_DIRECTORY=$(DOC_BUILD_DIR) \
	& echo WARN_LOGFILE=$(DOC_BUILD_DIR)/warnings.log \
	& echo PROJECT_NUMBER="Build project: $(PROJECT_NAME)" \
	& echo LAYOUT_FILE=$(subst /,\,$(DOCGENERATOR_LAYOUT_FILE)) \
	& echo EXCLUDE=$(PROJECT_ROOT_DIR)/smack_app/test/support/fake_otp.c \
				   $(PROJECT_ROOT_DIR)/smack_app/test/support/fake_otp.h \
				   $(PROJECT_ROOT_DIR)/smack_app/test/support/fake_otp_drv.c \
				   $(PROJECT_ROOT_DIR)/smack_app/test/support/fake_otp_drv.h \
				   $(PROJECT_ROOT_DIR)/smack_app/test/support/test_helper_exception.h \
	& echo GENERATE_XML=NO \
	& echo GENERATE_QHP=NO \
	& echo EXAMPLE_PATH=$(PROJECT_ROOT_DIR) \
) | cmd /c $(subst /,\,$(DOXYGEN_TOOL_DIR))\doxygen - >$(DOC_OUTPUT_LOG_FILE)

# When building the pdf, avoid settings that increase the size of the pdf excessively.
# - REPEAT_BRIEF=NO: doxygen will not prepend the brief description of a member or function before the detailed
#                    description
# - SHOW_FILES=NO: disable the generation of the Files page. This will remove the Files entry from the Quick Index and
#                  from the Folder Tree View.
# - SOURCE_BROWSER=NO: Since we do not want to include source code, we do not need to generate a list of source files
# - VERBATIM_HEADER=NO: doxygen will not generate a verbatim copy of the header file for each class
# - SHOW_USED_FILES=NO: disable the list of files generated at the bottom of the documentation of classes and structs.
# - EXCLUDE test: AIM not need to contain the unit test documentation
#           inc/gen: AIM does not need the register bitfields and enumerations, too much, only relevant for actual
#                    source code implementation => html only.
# - when using dot, many of the graphs cannot be converted properly yet.
#   - CLASS_GRAPH .. GENERATE_LEGEND
#
# todo RL, discuss with Gary how to cut from a certain hierarchy level downwards.
doc_pdf_aim: DOCGENERATOR = \
( type $(subst /,\,$(DOCGENERATOR_CONFIG_FILE)) \
	& echo INPUT=$(DOCGENERATOR_SOURCE_DIRS) \
	& echo IMAGE_PATH=$(DOCGENERATOR_SOURCE_DIRS) \
	& echo OUTPUT_DIRECTORY=$(DOC_BUILD_DIR) \
	& echo WARN_LOGFILE=$(DOC_BUILD_DIR)/warnings.log \
	& echo PROJECT_NUMBER="Build project: $(PROJECT_NAME)" \
	& echo LAYOUT_FILE=$(subst /,\,$(DOCGENERATOR_LAYOUT_FILE)) \
	& echo EXCLUDE=$(PROJECT_ROOT_DIR)/libs/dp3_lib/test \
                   $(PROJECT_ROOT_DIR)/libs/dp3_lib/inc/gen \
                   $(PROJECT_ROOT_DIR)/dp3_app/test \
	& echo REPEAT_BRIEF=NO \
	& echo SHOW_FILES=NO \
	& echo SHOW_USED_FILES=NO \
	& echo SOURCE_BROWSER=NO \
	& echo VERBATIM_HEADERS=NO \
	& echo CLASS_GRAPH=NO \
	& echo COLLABORATION_GRAPH=NO \
	& echo GROUP_GRAPHS=NO \
	& echo INCLUDE_GRAPH=NO \
	& echo INCLUDED_BY_GRAPH=NO \
	& echo GRAPHICAL_HIERARCHY=NO \
	& echo DIRECTORY_GRAPH=NO \
	& echo GENERATE_LEGEND=NO \
	& echo WARN_AS_ERROR= NO \
) | cmd /c $(subst /,\,$(DOXYGEN_TOOL_DIR))\doxygen - >$(DOC_OUTPUT_LOG_FILE)

DOC_FILE := $(DOC_BUILD_DIR)/html/index.html

# Build & Cleanup labels ===============================================
.PHONY: help_dox
help: help_dox
help_dox:
	@$(ECHO) 'make clean_doc        Remove $(DOC_BUILD_DIR)'
	@$(ECHO) 'make doc              Create documentation'
	@$(ECHO) 'make doc_pdf_aim      Create PDF documentation (via DITA): architecture and implementation manual'

.PHONY: doc
doc: $(DOC_FILE)

check_dir = @if exist ${1} (echo ${1} available) else (echo directory ${1} not found. ${2} & exit 1)

DOXY2PRISMA_TOOL_DIR:=$(REPO_ROOT_DIR)/tools/extern/doxygen2prisma
# define GIT_DIR to find curl
GIT_DIR:=$(REPO_ROOT_DIR)/tools/extern/git_bin
# extend path variable to find curl
PATH:=$(GIT_DIR);$(PATH)

.PHONY: doc_dita
doc_dita: doc
	do_doc_dita.bat $(DOXY2PRISMA_TOOL_DIR) $(DOC_BUILD_DIR) $(DOCGENERATOR_DOCINFO_FILE) $(PLANTUML_INCLUDE_PATH) >>$(DOC_OUTPUT_LOG_FILE)

.PHONY: doc_pdf_aim
doc_pdf_aim: doc_dita
	do_doc_pdf.bat $(DOXY2PRISMA_TOOL_DIR) $(DOC_BUILD_DIR) >>$(DOC_OUTPUT_LOG_FILE)
	@$(ECHO) Find the PDF output in $(DOC_BUILD_DIR)/pdf-infineon/toc.pdf

.PHONY: clean_doc
clean_doc:
	@$(ECHO) Removing "$(DOC_BUILD_DIR)".
	$(V)$(V)$(RMDIR) $(DOC_BUILD_DIR)

# Targets ==============================================================

# Create the build output directories.
$(DOC_BUILD_DIR): $(BUILD_DIR)
	$(V)$(MKDIR) $@

$(DOC_FILE): $(DOCGENERATOR_FILES) | $(DOC_BUILD_DIR)
	@$(ECHO) Generating the documentation (output goes to $(DOC_OUTPUT_LOG_FILE))
	$(V)$(DOCGENERATOR)
	@$(ECHO) Check the log "$(DOC_BUILD_DIR)/warnings.log"
