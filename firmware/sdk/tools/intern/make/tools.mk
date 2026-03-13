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

ifeq ($(PYTHON), ) # if empty
    $(error PYTHON is empty)
endif

ifeq ($(ECHO), ) # if empty
    $(error ECHO is empty)
endif

ifeq ($(SCRIPT_DIR), ) # if empty
    $(error SCRIPT_DIR is empty)
endif

ifeq ($(UTILS_DIR), ) # if empty
    $(error UTILS_DIR is empty)
endif

ifneq ("$(BUILD_DIR)", "")
$(BUILD_DIR): tools
endif

# List of Tool ZIP Packages
TOOL_PKGS := $(wildcard $(UTILS_DIR)/*.zip)
# Each tool has a corresponding tool directory with the same basename
TOOL_DIR := $(basename $(TOOL_PKGS))
$(foreach pkg, $(TOOL_PKGS), $(eval $(basename $(pkg)) : $(pkg)))

.PHONY: tools $(TOOL_DIR)
tools: $(TOOL_DIR)
$(TOOL_DIR):
#SDK already has tools extracted, skip action here
#	$(V)$(PYTHON) $(SCRIPT_DIR)/tool_extractor.py $(UTILS_DIR) $<

help: tools_help
.PHONY: tools_help
tools_help:
	$(V)echo 'make tools            Unzip tool packages'
