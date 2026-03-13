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

# perl tasks moved to UNIX
#PERL := C:\Strawberry\perl\bin\perl.exe

# Repo root directory, related to the project directory. This variable
# is used as reference when generating the absolute path information
ifeq ($(REPO_ROOT_DIR), ) # if empty
    $(error REPO_ROOT_DIR is empty)
endif

# Check the essential PROJECT_SOURCE_DIRS which contain the files for building anything
ifeq ($(PROJECT_SOURCE_DIRS), ) # if empty
    $(error PROJECT_SOURCE_DIRS is empty)
endif

# Check the essential PROJECT_APARAM_SOURCE_DIRS which contain the files for building anything
#ifeq ($(PROJECT_APARAM_SOURCE_DIRS), ) # if empty
#    $(error PROJECT_APARAM_SOURCE_DIRS is empty)
#endif

# Check the essential APARAM_INST_NAME which contain the files for building anything
#ifeq ($(APARAM_INST_NAME), ) # if empty
#    $(error APARAM_INST_NAME is empty)
#endif
# Check the essential APARAM_TYPE_NAME which contain the files for building anything
#ifeq ($(APARAM_TYPE_NAME), ) # if empty
#    $(error APARAM_TYPE_NAME is empty)
#endif

# Setting of names
OBJECT_BUILD_DIR := $(IMAGE_BUILD_DIR)/objects
APARAM_OBJECT_BUILD_DIR := $(APARAM_IMAGE_BUILD_DIR)/objects
DPARAM_OBJECT_BUILD_DIR := $(DPARAM_IMAGE_BUILD_DIR)/objects

# find all (!) C/CPP source files in the given source dirs
ALL_CC_FILES := $(foreach dir, $(PROJECT_SOURCE_DIRS:%/=%), $(wildcard  $(dir)/*.c  $(dir)/*.cpp) )

# find all (!) .inc source files in the given aparam source dir
#APARAM_CC_FILES = $(sort $(foreach file, $(notdir $(APARAM_SOURCE_FILES)), \
#               $(IMAGE_BUILD_DIR)/$(basename $(file)).elf))
APARAM_CC_FILES := $(APARAM_SOURCE_FILES)
DPARAM_CC_FILES := $(DPARAM_SOURCE_FILES)

# if required/desired, apply black-listing to list of all source files
#CC_FILES := $(filter-out blacklisted_source_files, $(ALL_CC_FILES))
CC_FILES := $(ALL_CC_FILES)

#$(info list of to-be-compiled C source files:  $(CC_FILES))

# find all (!) assembly source files in the given source dirs
ASM_FILES := $(foreach dir, $(PROJECT_SOURCE_DIRS:%/=%), $(wildcard  $(dir)/*.S) )

# Create a list of all object files including their path to the build dir from the source files. 
# Sort the list of object files for the linker to make sure the build output is the same independent
# of the sorting algorithm of the used make tool.
ALL_OBJECT_FILES = $(sort $(foreach file, $(notdir $(CC_FILES)) $(notdir $(ASM_FILES)), \
               $(OBJECT_BUILD_DIR)/$(basename $(file)).o))

# Create a list of all aparam and dparam object files including their path to the build dir from the source files. 
# Sort the list of object files for the linker to make sure the build output is the same independent
# of the sorting algorithm of the used make tool.
APARAM_OBJECT_FILES = $(sort $(foreach file, $(notdir $(APARAM_CC_FILES)), \
               $(APARAM_OBJECT_BUILD_DIR)/$(basename $(file)).o))
DPARAM_OBJECT_FILES = $(sort $(foreach file, $(notdir $(DPARAM_CC_FILES)), \
               $(DPARAM_OBJECT_BUILD_DIR)/$(basename $(file)).o))

#$(info list of all object files:  $(ALL_OBJECT_FILES))
# if required/desired, apply black-listing to list of all object files
#LINKER_FILES = $(filter-out $(OBJECT_BUILD_DIR)/xxx.o, $(ALL_OBJECT_FILES))
#LINKER_FILES = $(filter-out $(OBJECT_BUILD_DIR)/pfc_qrm.o, $(ALL_OBJECT_FILES))
# no black-listing needed ...
LINKER_FILES = $(ALL_OBJECT_FILES)
APARAM_LINKER_FILES = $(APARAM_OBJECT_FILES)
DPARAM_LINKER_FILES = $(DPARAM_OBJECT_FILES)
#$(info list of all to-be-linked object files:  $(LINKER_FILES))

# Create the filenames for the dependency files out of the .o files' names.
# The dependency files contain the .h files as prerequesites for the .o files
# The according rules look like "$(OBJECT_BUILD_DIR)/%.o: path/header.h".
# "-include" is used to make sure that GNUmake does not complain if a *.d file does not (yet) exist.
-include $(filter %.d, $(patsubst %.o, %.d, $(LINKER_FILES)))
-include $(filter %.d, $(patsubst %.o, %.d, $(APARAM_LINKER_FILES)))
-include $(filter %.d, $(patsubst %.o, %.d, $(DPARAM_LINKER_FILES)))

# Use VPATH to locate the source and header files for the wildcard prerequesites (e.g. %.c).
VPATH := $(PROJECT_SOURCE_DIRS) $(PROJECT_APARAM_SOURCE_DIRS) $(PROJECT_DPARAM_SOURCE_DIRS)

###################################################################################################
# Tool Configuration (executables, parameters)
###################################################################################################

# Compiler ========================================================================================
CC := arm-none-eabi-gcc

# cmd line options;
# -MF                             Dependency output file
# -MMD                            Dependency output file mention only user header files,
#                                 not system header files.
# -MP                             Dependency output file: This option instructs CPP
#                                 to add a phony target for each dependency other
#                                 than the main file, causing each to depend on nothing.
#                                 These dummy rules work around errors make gives
#                                 if you remove header files without updating
#                                 the Makefile to match.
# -MT                             Dependency output file: Change the target of the rule
#                                 emitted by dependency generation.
# -Os                             Optimization for space (default for all sources)
# -Wall                           Enable all warnings
# -Wno-unused-label               Disable warning about unused labels. 
#                                 We allow unused labels so that the debugger can place 
#                                 symbolic breakpoints on arbitrary lines in the code.
# -Wa,-adhlns                     ???
# -c                              Compile or assemble the source files, but do not link.
#                                 The compiler output is an object file corresponding
#                                 to each source file.
# -fdata-sections                 Place global variables in separate linker sections
# -ffunction-sections             Place functions in separate linker sections
# -fmessage-length=0              Try to format error messages so that they fit on lines
#                                 of about n characters. The default is 72 characters
#                                 for g++ and 0 for the rest of the front ends
#                                 supported by GCC. If n is zero, then no line-wrapping
#                                 will be done; each error message will appear
#                                 on a single line.
# -g                              Produce debugging information
# -gdwarf-2                       Produce debugging information in DWARF format
# -mcpu=cortex-m0                 Cortex Device
# -mthumb                         Thumb instruction mode
# -pipe                           Use pipes rather than temporary files for
#                                 communication between the various stages
#                                 of compilation.
# -std=gnu99                      C99 Standard
# -std=gnu11                      C11 Standard
# -v                              verbose mode, prints the complete cmd line and lots of more data
#                                 in case you need to _exactly_ know what gcc is doing.
#
# Note: Make sure that COMPILER_PARAMS is only expanded when used ("=") and not when
# defined (":=") so that $@ will be evaluated correctly.
#
# first check for debug build and forward command line setting to compiler
ifeq ($(DEBUG), )
CC_PARAMS += -Os
else
CC_PARAMS += -Og -DDEBUG=$(DEBUG)
endif
CC_PARAMS += $(addprefix -I, $(PROJECT_HEADER_DIRS)) \
             -ffunction-sections -fdata-sections -Wall \
             -Wno-unused-label \
             -std=gnu99 -Wa,-adhlns="$(basename $@).lst" \
             -pipe -c -fmessage-length=0 \
             -MMD -MP \
             -MT"$(basename $@).d $@" \
             -MF"$(basename $@).d" \
             -mcpu=cortex-m0 -mthumb -g -gdwarf-2

# Assembler =======================================================================================
ASM := arm-none-eabi-gcc

# Assembler
# cmd line options:
# -MF                             Dependency output file
# -MMD                            Dependency output file mention only user header files,
#                                 not system header files.
# -MP                             Dependency output file: This option instructs CPP
#                                 to add a phony target for each dependency other
#                                 than the main file, causing each to depend on nothing.
#                                 These dummy rules work around errors make gives
#                                 if you remove header files without updating
#                                 the Makefile to match.
# -MT                             Dependency output file: Change the target of the rule
#                                 emitted by dependency generation.
# -Wall                           Enable all warnings
# -Wa,-adhlns                     ???
# -c                              Compile or assemble the source files, but do not link.
#                                 The compiler output is an object file corresponding
#                                 to each source file.
# -fmessage-length=0              ???
# -g                              Produce debugging information
# -gdwarf-2                       Produce debugging information in DWARF format
# -mcpu=cortex-m0                 Cortex Device
# -mthumb                         Thumb instruction mode
ASM_PARAMS += $(addprefix -I, $(PROJECT_HEADER_DIRS)) \
              -Wall \
              -Wa,-adhlns="$(basename $@).lst" \
              -c -fmessage-length=0 -MMD -MP \
              -MF"$(basename $@).d" -MT"$(basename $@).d $@" \
              -mcpu=cortex-m0 -mthumb

# Linker ==========================================================================================
LINKER := arm-none-eabi-gcc

# Linker requires a linker configuration file
ifeq ($(LINKER_CONFIG_ROM_FILE), ) # if empty
    $(error LINKER_CONFIG_ROM_FILE is empty)
endif

# Linker prerequisites
LINKER_PREREQUISITE_FILES := $(foreach dir, $(LINKER_INCLUDE_DIRS:%/=%), $(wildcard  $(dir)/*.ld) )

# cmd line options
# Note: Make sure that LINKER_PARAMS is only expanded when used ("=") and not when
# defined (":=") so that $@ will be evaluated.
# -Wl                             All linker-specific options (e.g. --gc-sections) need
#                                 to be prefixed with -Wl because we do not call the linker
#                                 directly ('ld'), but through the driver ('arm-none-eabi-gcc').
#                                 If -Wl is missing, linker options may be silently dropped.
# -T                              Linker script file
# -Map                            Output map file
# -nostartfiles                   Do not use the standard system startup
#                                 files when linking. The standard libraries
#                                 are used normally.
# -l                              Library files to add for during the target linking process
LINKER_INCLUDE_PREFIX := -Wl,-L,
LINKER_PARAMS += -nostartfiles \
                 $(addprefix $(LINKER_INCLUDE_PREFIX), $(LINKER_INCLUDE_DIRS)) \
                 -Wl,-Map,$(basename $@).map -mcpu=cortex-m0 -mthumb

# TODO [SR]: The following linker parameter is temporary disabled to keep all target image linker symbols.
#            Do not remove unused variables and functions. Add this parameter later again and ensure that
#            important library functions are not removed during the ROM linking process.
# --gc-sections                   enable garbage collection of unused input sections.
#                                 unused sections that need to be kept anyway need to be
#                                 marked with the KEEP directive in the linker script file.
#LINKER_PARAMS += -Wl,--gc-sections -Wl,--print-gc-sections

LINKER_LIBRARY_PREFIX := -Wl,-l
LINKER_PARAMS_LIB += $(addprefix $(LINKER_LIBRARY_PREFIX), $(LINKER_LIBRARIES))
LINKER_LIBRARY_FILES += $(foreach lib, $(LINKER_LIBRARIES), $(wildcard $(addprefix $(REPO_ROOT_DIR)/smack_lib*/build/image/lib, $(lib:%=%.a))))

# Code size information ===========================================================================
SIZER := arm-none-eabi-size
SIZER_PARAMS += --format=berkeley
FUNCTION_SIZER := $(REPO_ROOT_DIR)/tools/extern/gcc-arm-none-eabi/arm-none-eabi/bin/nm 
FUNCTION_SIZER_PARAMS += --radix x -S --size-sort --reverse-sort --line-numbers

# Archiver ==========================================================================================
ARCHIVER := arm-none-eabi-gcc-ar

# cmd line options
# r                               Insert the files member... into archive (with replacement).
# c                               Create the archive
# s                               Write an object-file index into the archive or update an existing one.
ARCHIVER_PARAMS += rcs

###################################################################################################
# Targets
###################################################################################################

# Compile every single source file into an object file with the same basename.
# The compiles output file is pre-linked to mangle the global symbols by a Keil
# specific steering configuration. This is needed for patching to hide compiled symbols and
# use the ROM reference instead.
$(OBJECT_BUILD_DIR)/%.o: %.c | $(OBJECT_BUILD_DIR)
ifneq ("$(ASYTLE)", "")
	@$(ECHO) Code Formating for "$<"
	$(V)$(ASYTLE) $(ASYTLE_PARAMS) --files $^
endif
	@$(ECHO) Compiling "$<"
	$(V)$(CC) $(CC_PARAMS) -o $@ $<

$(OBJECT_BUILD_DIR)/%.o: %.S | $(OBJECT_BUILD_DIR)
	@$(ECHO) Assembling "$<"
	$(V)$(ASM) $(ASM_PARAMS) -o $@ $<

# Compile every single aparam source file into an object file with the same basename.
$(APARAM_OBJECT_BUILD_DIR)/%.o: %.c | $(APARAM_OBJECT_BUILD_DIR)
ifneq ("$(ASYTLE)", "")
	@$(ECHO) Code Formating for "$<"
	$(V)$(ASYTLE) $(ASYTLE_PARAMS) --files $^
endif
	@$(ECHO) Compiling "$<"
	$(V)$(CC) $(CC_PARAMS) -o $@ $<

# Compile every single dparam source file into an object file with the same basename.
$(DPARAM_OBJECT_BUILD_DIR)/%.o: %.c | $(DPARAM_OBJECT_BUILD_DIR)
ifneq ("$(ASYTLE)", "")
	@$(ECHO) Code Formating for "$<"
	$(V)$(ASYTLE) $(ASYTLE_PARAMS) --files $^
endif
	@$(ECHO) Compiling "$<"
	$(V)$(CC) $(CC_PARAMS) -o $@ $<

# Linking all object together to a static library.
image_rom: $(ARCHIVER_FILE)
image_nvm: $(ARCHIVER_FILE)
image_lib: $(ARCHIVER_FILE)
image_ram: $(ARCHIVER_FILE)
$(ARCHIVER_FILE): $(LINKER_FILES)
	@$(ECHO) Generate Library "$@"
	$(V) $(RMALL) $@
	$(V) $(ARCHIVER) $(ARCHIVER_PARAMS) $@ $(LINKER_FILES)

# Link the object files together and generate the executable file.
# Beforehand always perform a static C code analysis.
# @todo AP outdated? We don't do static analysis here ...
# Note: No duplicate basenames are allowed. If desired the prerequesites of
# the source files have to be separated for their respective targets.
# -T                              Linker script file

# Linkage of ROM image
$(LINKED_IMAGE_ROM_FILE): $(LINKER_FILES) $(LINKER_CONFIG_ROM_FILE) $(LINKER_PREREQUISITE_FILES)
	@$(ECHO) Linking object files and building "$@"
	$(V)$(LINKER) $(LINKER_PARAMS) -T"$(LINKER_CONFIG_ROM_FILE)" $(LINKER_FILES) -o $@

# Linkage of NVM image
$(LINKED_IMAGE_NVM_FILE): $(LINKER_FILES) $(LINKER_CONFIG_NVM_FILE) $(LINKER_PREREQUISITE_FILES) $(LINKER_LIBRARY_FILES)
	@$(ECHO) Linking object files and building "$@"
	$(V)$(LINKER) $(LINKER_PARAMS) -T"$(LINKER_CONFIG_NVM_FILE)" $(LINKER_FILES) $(LINKER_PARAMS_LIB) -o $@

# Linkage of RAM image
$(LINKED_IMAGE_RAM_FILE): $(LINKER_FILES) $(LINKER_CONFIG_RAM_FILE) $(LINKER_PREREQUISITE_FILES) $(LINKER_LIBRARY_FILES)
	@$(ECHO) Linking object files and building "$@"
	$(V)$(LINKER) $(LINKER_PARAMS) -T"$(LINKER_CONFIG_RAM_FILE)" $(LINKER_FILES) $(LINKER_PARAMS_LIB) -o $@

# Linkage of APARAM images
$(APARAM_IMAGE_BUILD_DIR)/%.elf: $(APARAM_OBJECT_BUILD_DIR)/%.o $(LINKER_CONFIG_ROM_FILE) $(LINKED_IMAGE_ROM_FILE) $(LINKER_PREREQUISITE_FILES)
	@$(ECHO) Linking aparam object files and building "$@"
	$(V)$(LINKER) $(LINKER_PARAMS) -T"$(LINKER_CONFIG_DPARAM_FILE)" $(APARAM_OBJECT_BUILD_DIR)/$*.o -o $@

# Linkage of DPARAM images
#$(DPARAM_IMAGE_BUILD_DIR)/%.elf: $(DPARAM_OBJECT_BUILD_DIR)/%.o $(LINKER_CONFIG_ROM_FILE) $(LINKED_IMAGE_ROM_FILE)
#	@$(ECHO) Linking dparam object files and building "$@"
#	$(V)$(LINKER) $(LINKER_PARAMS) -T"$(LINKER_CONFIG_ROM_FILE)" $(DPARAM_OBJECT_BUILD_DIR)/$*.o -o $@
$(DPARAM_IMAGE_BUILD_DIR)/%.elf: $(DPARAM_OBJECT_BUILD_DIR)/%.o $(LINKER_CONFIG_DPARAM_FILE) $(LINKED_IMAGE_ROM_FILE) $(LINKER_PREREQUISITE_FILES)
	@$(ECHO) Linking dparam object files and building "$@"
	$(V)$(LINKER) $(LINKER_PARAMS) -T"$(LINKER_CONFIG_DPARAM_FILE)" $(DPARAM_OBJECT_BUILD_DIR)/$*.o -o $@

# Export of DPARAM images
$(DPARAM_IMAGE_BUILD_DIR)/%.hex: $(DPARAM_OBJECT_BUILD_DIR)/%.elf
	@$(ECHO) Creating an intel hex file from "$<"
	$(V)$(OBJCOPY) -O ihex $< $@

dump:
	@echo APARAM_IMAGE_BUILD_DIR  = $(APARAM_IMAGE_BUILD_DIR)
	@echo APARAM_OBJECT_BUILD_DIR = $(APARAM_OBJECT_BUILD_DIR)
	@echo DPARAM_IMAGE_BUILD_DIR  = $(DPARAM_IMAGE_BUILD_DIR)
	@echo DPARAM_OBJECT_BUILD_DIR = $(DPARAM_OBJECT_BUILD_DIR)

# Prepare different output formats ===============================================================
OBJDUMP := arm-none-eabi-objdump
OBJCOPY := arm-none-eabi-objcopy

# Get a textual representation of the target image.
#
# The prerequisite for obtaining a target called 'image_rom' is a .txt file.
# @todo Technically, this is nad naming: 'image_rom' as target name is saying
# that it will give you an 'image'. An image is an executable (.elf, .a, .exe, ...)
# But here, 'image_rom' says sth fundamentally different: When building 'image_rom'
# we also want a .txt (== human readable version) from the .elf file.
# => image_rom is the set of {image_rom.elf, .txt, .hex, .bin, rom crc, ...}.
# => The name of the target should be sth like 'fw'
image_rom: $(IMAGE_BUILD_DIR)/$(IMAGE_ROM_NAME).txt
image_nvm: $(IMAGE_BUILD_DIR)/$(IMAGE_NVM_NAME).txt
image_ram: $(IMAGE_BUILD_DIR)/$(IMAGE_RAM_NAME).txt
# Tell make how convert .elf into .txt, thereby telling it how
# to fulfill the above dependency.
%.txt: %.elf
	@$(ECHO) Creating a textual representation of "$<"
	$(V)$(OBJDUMP) -h -S $< > $@

# Create an intel hex file from the target image.
image_rom: $(IMAGE_BUILD_DIR)/$(IMAGE_ROM_NAME).hex
image_nvm: $(IMAGE_BUILD_DIR)/$(IMAGE_NVM_NAME).hex
image_ram: $(IMAGE_BUILD_DIR)/$(IMAGE_RAM_NAME).hex
#image_rom: $(IMAGE_BUILD_DIR)/$(IMAGE_ROM_NAME).elf
#           $(DPARAM_IMAGE_BUILD_DIR)/%.hex
%.hex: %.elf
	@$(ECHO) Creating an intel hex file from "$<"
	$(V)$(OBJCOPY) -O ihex $< $@

# Create an binary file from the target image.
image_rom: $(IMAGE_BUILD_DIR)/$(IMAGE_ROM_NAME).bin
image_nvm: $(IMAGE_BUILD_DIR)/$(IMAGE_NVM_NAME).bin
image_ram: $(IMAGE_BUILD_DIR)/$(IMAGE_RAM_NAME).bin
%.bin: %.elf
	@$(ECHO) Creating a binary file from "$<"
	$(V)$(OBJCOPY) -O binary $< $@

# Create alternative ROM image files for RTL simulation from the target ROM image. - Moved to UNIX env
#image_rom: $(IMAGE_BUILD_DIR)/$(IMAGE_ROM_NAME).rom
#%.rom: %.hex
#	@$(ECHO) $(___TARGET_SEPARATOR_LINE___)
#	@$(ECHO) Creating alternative ROM file from "$<"
#	$(PERL) $(SCRIPT_DIR)/hex2mem.pl -i $< -o $@ -na -dw 4 -bd -size 4096 -num_mem 2 -par_mem 2 -init
#	$(PERL) $(SCRIPT_DIR)/hex2mem.pl -i $< -o $(@).href -dw 4 -aw 8 -size 16384
#	touch $@
	
###################################################################################################
# Build & Cleanup labels
###################################################################################################

# Create the build output directories.
# Note: Do not add them as prerequesites for the object files or image file since if you do so
# these files will be rebuild as soon as the directories are marked as changed. And this will
# happen as soon as one of the files within the directories is rebuilt.

# Target BUILD_DIR has its rule in env.mk. Create the two subdirectories here

$(IMAGE_BUILD_DIR): $(BUILD_DIR)
	$(V)$(MKDIR) $@

$(OBJECT_BUILD_DIR): $(IMAGE_BUILD_DIR)
	$(V)$(MKDIR) $@

$(APARAM_IMAGE_BUILD_DIR): $(BUILD_DIR)
	$(V)$(MKDIR) $@

$(APARAM_OBJECT_BUILD_DIR): $(APARAM_IMAGE_BUILD_DIR)
	$(V)$(MKDIR) $@

$(DPARAM_IMAGE_BUILD_DIR): $(BUILD_DIR)
	$(V)$(MKDIR) $@

$(DPARAM_OBJECT_BUILD_DIR): $(DPARAM_IMAGE_BUILD_DIR)
	$(V)$(MKDIR) $@

.PHONY: image_rom
image_rom: $(IMAGE_BUILD_DIR) $(APARAM_IMAGE_BUILD_DIR) $(DPARAM_IMAGE_BUILD_DIR) $(TARGET_IMAGE_ROM_FILE)
	$(V)$(SIZER) $(SIZER_PARAMS) $(LINKER_FILES)
	$(V)$(FUNCTION_SIZER) $(FUNCTION_SIZER_PARAMS) $(IMAGE_BUILD_DIR)/$(IMAGE_ROM_NAME).elf > $(IMAGE_BUILD_DIR)/$(IMAGE_ROM_NAME)_functions_size.txt
	$(V)$(PYTHON) $(SCRIPT_DIR)/elfsize.py -t rom -e $(TARGET_IMAGE_ROM_FILE)
	@$(ECHO) Finished building "$(TARGET_IMAGE_ROM_FILE)".

.PHONY: image_nvm
image_nvm: $(IMAGE_BUILD_DIR) $(APARAM_IMAGE_BUILD_DIR) $(DPARAM_IMAGE_BUILD_DIR) $(TARGET_IMAGE_NVM_FILE)
	$(V)$(SIZER) $(SIZER_PARAMS) $(LINKER_FILES)
	$(V)$(FUNCTION_SIZER) $(FUNCTION_SIZER_PARAMS) $(IMAGE_BUILD_DIR)/$(IMAGE_NVM_NAME).elf > $(IMAGE_BUILD_DIR)/$(IMAGE_NVM_NAME)_functions_size.txt
ifeq ($(SDK), 0)
	$(V)$(PYTHON) $(SCRIPT_DIR)/elfsize.py -t nvm -e $(TARGET_IMAGE_NVM_FILE)
endif
	@$(ECHO) Finished building "$(TARGET_IMAGE_NVM_FILE)".

.PHONY: image_ram
image_ram: $(IMAGE_BUILD_DIR) $(APARAM_IMAGE_BUILD_DIR) $(DPARAM_IMAGE_BUILD_DIR) $(TARGET_IMAGE_RAM_FILE)
	$(V)$(SIZER) $(SIZER_PARAMS) $(LINKER_FILES)
	$(V)$(FUNCTION_SIZER) $(FUNCTION_SIZER_PARAMS) $(IMAGE_BUILD_DIR)/$(IMAGE_RAM_NAME).elf > $(IMAGE_BUILD_DIR)/$(IMAGE_RAM_NAME)_functions_size.txt
	$(V)$(PYTHON) $(SCRIPT_DIR)/elfsize.py -t ram -e $(TARGET_IMAGE_RAM_FILE)
	@$(ECHO) Finished building "$(TARGET_IMAGE_RAM_FILE)".

.PHONY: clean_image
clean_image:
	@$(ECHO) Removing "$(IMAGE_BUILD_DIR)".
	$(V)$(RMDIR) $(IMAGE_BUILD_DIR)
	@$(ECHO) Removing "$(APARAM_IMAGE_BUILD_DIR)".
	$(V)$(RMDIR) $(APARAM_IMAGE_BUILD_DIR)
	@$(ECHO) Removing "$(DPARAM_IMAGE_BUILD_DIR)".
	$(V)$(RMDIR) $(DPARAM_IMAGE_BUILD_DIR)

###################################################################################################
# Extension targets
###################################################################################################

###################################################################################################
# Display the help menue
###################################################################################################
# If this is the first target (not overruled in the Makefile) and it will be called when no
# argument is given to GNUmake. The target specific help relies on the general help target and
# extends it by the target specific parts.
help: gcc_compile_help
.PHONY: gcc_compile_help
gcc_compile_help:
	@$(ECHO) 'make image_rom        Create the firmware ROM image for Smack ROM in $(IMAGE_BUILD_DIR)'
	@$(ECHO) 'make image_nvm        Create the firmware ROM image for Smack NVM in $(IMAGE_BUILD_DIR)'
	@$(ECHO) 'make image_ram        Create the firmware RAM image for Smack RAM in $(IMAGE_BUILD_DIR)'
	@$(ECHO) 'make clean_image      Remove $(IMAGE_BUILD_DIR)'
