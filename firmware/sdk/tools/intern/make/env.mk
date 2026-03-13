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

# Check for relevant variables
ifeq ($(REPO_ROOT_DIR), ) # if empty
    $(error REPO_ROOT_DIR is empty)
endif

# control verbosity of all commands
V=@

# define if this is SDK
SDK := 1

# Determine the OS based on standard environment variables.
# Note that if you are running under BASH, make sure that the BASH variable is
# marked for export in your .bashrc file in your home directory.
ifeq ("$(PROGRAMFILES)$(ProgramFiles)", "")
    OS = Linux
else
    OS = Windows
endif

# The objective of the following lines is to make sure that the actual build
# is executed within a suitable, well-defined shell along with a usable, but minimal path.
# Doing this, we avoid any interference of the make process with a shell that is
# inherited from the actual user.
ifeq ("$(OS)", "Linux")

    SHELL := bash

    # Installation directory for the image build (see target "image_rom")
    TOOLCHAIN_DIR := /opt/gccarm/gcc-arm-none-eabi-5_2-2015q4/bin

    # Installation directory of the C code analyzer (see target "codeanalysis")
    CCODEANALYZER_DIR := # unused. deactivated in the target $(IMAGE_FILE)

    # Doxygen documentation tool (see target "doc")
    # Under Linux, doxygen is loaded into the user's environment, so we assume it is
    # available anywhere and the path to the executable is empty.
    #
    # We need to make sure that 'module load doxygen' has been done before
    # running doxygen. I tried to integrate it into this makefile, but failed and
    # did not follow up.
    DOXYGEN_DIR :=

    # Defines operating system specific helper macros
    COPY    := cp --update # copy when source is newer than destination
    ECHO    := echo
    MKDIR   := mkdir -p
    RMALL   := rm -f
    RMDIR   := rm -fr
    MV      := mv -f

    # Python Interpretor
    PYTHON  := python

    export PATH := $(PATH):$(TOOLCHAIN_DIR)

else # Windows

    # Tool directories
    UTILS_DIR := $(REPO_ROOT_DIR)/tools/extern
    SCRIPT_DIR := $(REPO_ROOT_DIR)/scripts

    # =======================================================================
    # Helper Commands
    # =======================================================================

    # Make sure that any commands initiated by make are done within the default
    # windows shell. This makes sure that make does not use e.g. a cygwin shell
    # that it finds in the path by chance.
    SHELL := C:/Windows/system32/cmd.exe

    # Installation directory of the build tools for the image (see target "image_rom")
    TOOLCHAIN_DIR := $(UTILS_DIR)/gcc-arm-none-eabi/bin
    # Installation directory of the C code analyzer (see target "codeanalysis")
    CCODEANALYZER_DIR := $(UTILS_DIR)/pclint8.00
    # Host GCC compiler
    HOST_GCC_DIR := $(UTILS_DIR)/gcc/bin
    # Python interpreter
    PYTHON_DIR := $(UTILS_DIR)/python2.7.9.10
    # Make
    MAKE_DIR := $(UTILS_DIR)/make/bin
    # AStyle
    ASTYLE_DIR := $(UTILS_DIR)/AStyle

    CMD_DIR := C:/Windows/system32

    PATH := $(MAKE_DIR):$(PYTHON_DIR):$(HOST_GCC_DIR):$(TOOLCHAIN_DIR):$(CCODEANALYZER_DIR):$(ASTYLE_DIR):$(PATH)

    # Directory of additional GNUmake scripts under windows (batch files like
    # windows_gmake_mkdir.bat, usually in the same directory as GNUmake).

    # Defines operating system specific helper macros
    WIN_MAKE_DIR := $(REPO_ROOT_DIR)/tools/extern/make/bin
    COPY    := cmd /c $(subst /,\,$(WIN_MAKE_DIR)/windows_gmake_cp.bat)
    ECHO    := cmd /c echo
    MKDIR   := cmd /c $(subst /,\,$(WIN_MAKE_DIR)/windows_gmake_mkdir.bat)  # mkdir
    RMALL   := cmd /c $(subst /,\,$(WIN_MAKE_DIR)/windows_gmake_rm.bat)     # del /F /Q
    RMDIR   := cmd /c $(subst /,\,$(WIN_MAKE_DIR)/windows_gmake_rmdir.bat)  # rmdir /s /q
    MV      := cmd /c $(subst /,\,$(WIN_MAKE_DIR)/windows_gmake_mv.bat)     # ren

    # Python Interpretor
    PYTHON  := python.exe

	# Dot Path used by PlantUML
	export GRAPHVIZ_DOT = $(UTILS_DIR)/graphviz/bin/dot.exe

endif

###################################################################################################
# Targets
###################################################################################################

# Default target
help:

# Target rules to generate a build and remove a build directory are only needed in case the
# BUILD_DIR - variable is defined. This variable is not defined in case of integrator Makefiles,
# where the build targets only call sub-directory make processes.
ifneq ("$(BUILD_DIR)", "")
.PHONY: help_env
help: help_env
help_env:
	@$(ECHO) 'make clean            Remove $(BUILD_DIR)'

.PHONY: clean
clean:
	@$(ECHO) Removing "$(BUILD_DIR)".
	$(V)$(RMDIR) $(BUILD_DIR)

$(BUILD_DIR):
	$(V)$(MKDIR) $@
endif
