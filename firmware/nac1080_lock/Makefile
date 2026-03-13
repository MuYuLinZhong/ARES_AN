# ARES NAC1080 Lock Firmware Makefile
#
# Requires: GNU Arm Embedded Toolchain (arm-none-eabi-gcc)
# SDK Path: ../sdk

# Tool chain
CROSS_COMPILE ?= arm-none-eabi-
CC      = $(CROSS_COMPILE)gcc
AS      = $(CROSS_COMPILE)as
LD      = $(CROSS_COMPILE)ld
OBJCOPY = $(CROSS_COMPILE)objcopy
SIZE    = $(CROSS_COMPILE)size

# SDK paths
SDK_ROOT    = ../sdk
SMACK_LIB   = $(SDK_ROOT)/smack_lib
SMACK_ROM   = $(SDK_ROOT)/smack_rom
SMACK_SL    = $(SDK_ROOT)/smack_sl

# Directories
SRC_DIR = src
INC_DIR = inc
BUILD_DIR = build

# Include paths
INCLUDES = \
    -I$(INC_DIR) \
    -I$(SMACK_LIB)/inc \
    -I$(SMACK_ROM)/libs/smack_lib/inc \
    -I$(SMACK_ROM)/libs/CMSIS/inc \
    -I$(SMACK_ROM)/libs/CMSIS/ifx/smack_series/inc \
    -I$(SMACK_SL)/inc

# Sources
SRCS = \
    $(SRC_DIR)/lock_main.c \
    $(SRC_DIR)/lock_datapoints.c \
    $(SRC_DIR)/lock_auth.c \
    $(SRC_DIR)/lock_motor.c \
    $(SRC_DIR)/lock_aparam.c \
    $(SRC_DIR)/startup_smack.c

# Compiler flags
CFLAGS = -mcpu=cortex-m0 -mthumb -Os -g -Wall -Wextra \
         -ffunction-sections -fdata-sections \
         $(INCLUDES)

# ROM symbol references (provides __romcode_ram_end_, __romcode_ram2_end_, etc.)
ROM_IMAGE   = $(SMACK_ROM)/build/image/image_rom.elf
DPARAM_IMAGE = $(SMACK_ROM)/build/dparams/default_dparam.elf

# Linker flags
LDFLAGS = -mcpu=cortex-m0 -mthumb -Wl,--gc-sections \
          -L $(SMACK_ROM)/libs/smack_lib/inc \
          -L $(SMACK_LIB)/build/image \
          -T $(SMACK_SL)/src/Linker_config.ld \
          -Wl,--just-symbols,$(ROM_IMAGE) \
          -Wl,--just-symbols,$(DPARAM_IMAGE)

# Libraries to link (libsmack.a contains smack_exchange, AES, RNG, timer, etc.)
LIBS = -lsmack

# Output
TARGET = ares_lock
ELF = $(BUILD_DIR)/$(TARGET).elf
HEX = $(BUILD_DIR)/$(TARGET).hex
BIN = $(BUILD_DIR)/$(TARGET).bin

# Object files
OBJS = $(patsubst $(SRC_DIR)/%.c,$(BUILD_DIR)/%.o,$(SRCS))

.PHONY: all clean flash size

all: $(HEX) $(BIN) size

$(BUILD_DIR):
	mkdir -p $(BUILD_DIR)

$(BUILD_DIR)/%.o: $(SRC_DIR)/%.c | $(BUILD_DIR)
	$(CC) $(CFLAGS) -c $< -o $@

$(ELF): $(OBJS)
	$(CC) $(LDFLAGS) $(OBJS) $(LIBS) -o $@

$(HEX): $(ELF)
	$(OBJCOPY) -O ihex $< $@

$(BIN): $(ELF)
	$(OBJCOPY) -O binary $< $@

size: $(ELF)
	$(SIZE) $<

clean:
	rm -rf $(BUILD_DIR)

flash: $(HEX)
	@echo "Flash $(HEX) using your SWD debugger tool"
	@echo "Example: JLinkExe -device NAC1080 -if SWD -speed 4000 -CommandFile flash.jlink"
