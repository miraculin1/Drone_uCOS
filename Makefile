TARGET = os

BUILD_DIR = build
C_SRC = $(shell find . -name '*.c')

OPT =

# compile
PREFIX = arm-none-eabi-
CC = $(PREFIX)gcc
CP = $(PREFIX)objcopy
AS = $(PREFIX)gcc -x assembler-with-cpp
SZ = $(PREFIX)size

# from Truestudio
ASM_SRC = \
./startup_stm32f401xe.s \
./Ports/stm32f401/GNU/os_cpu_a.s \

# wheather debug DEBUG = 1
DEBUG = 1


###############
# CFLAGS
###############

#cpu
CPU = -mcpu=cortex-m4

# fpu
FPU = -mfpu=fpv4-sp-d16

# float-abi
FLOAT-ABI = -mfloat-abi=hard

# mcu
MCU = $(CPU) -mthumb $(FPU) $(FLOAT-ABI)

AS_DEFS =


C_DEFS = \
				 -DSTM32F401xx \

AS_INC =



C_INC = \
				-I./Source \
				-I./Ports/stm32f401/GNU \
				-I./Cfg/Template \
				-I/usr/arm-none-eabi/include \
				-I./Basic/Inc/ \
				-I./Source/drivers/Inc \
				-I./Source/trace/ \



# the "sections" options and the --gc-sections in link process
# is used to delete the dead code that not used
# they must be used at the same time

ASFLAGS = $(MCU) $(AS_DEFS) $(AS_INC) -Wall -fdata-sections -ffunction-sections

CFLAGS = $(MCU) $(C_DEFS) $(C_INC) $(OPT) -Wall -fdata-sections -ffunction-sections

# set debug flag
ifeq ($(DEBUG), 1)
CFLAGS += -g3
#optimize
OPT = \
			-Og
endif

# dependency output .d file to build dir
# mmd output .d file -mp will gen a phony target to
# make sure make no error when deleted .h file
CFLAGS += -MMD -MP -MF"$(BUILD_DIR)/$(notdir $(@:%.o=%.d))"

# link script
# this file define memory map/section stack location and size
LDSCRIPT = \
./STM32F401RETx_FLASH.ld \
# ./myld.ld \

#LIBS = -lc -lm -lnosys
# lc stand for standard c LIB
# lm stands for math LIB
# lnosys is from newlib(For embeded) not sure what for (for now)
LIBS = -lc -lnosys
LIBDIR = 

#Wl, pass things behind ',' to linker
# map seems store data useful for debug
# cref seems also have to do with debug
# --gc-sections delete the unused code
LDFLAG = $(MCU) -T$(LDSCRIPT) $(LIBDIR) $(LIBS) -Wl,-Map=$(BUILD_DIR)/$(TARGET).map,--cref -Wl,--gc-sections


all: $(BUILD_DIR)/$(TARGET).elf $(BUILD_DIR)/$(TARGET).hex $(BUILD_DIR)/$(TARGET).bin

# 我认为 vpath 是提示相关文件去“哪”找
OBJ = $(addprefix $(BUILD_DIR)/,$(notdir $(C_SRC:%.c=%.o)))

vpath %.c $(sort $(dir $(C_SRC)))

OBJ += $(addprefix $(BUILD_DIR)/,$(notdir $(ASM_SRC:%.s=%.o)))

vpath %.s $(sort $(dir $(ASM_SRC)))

# "|" then reqiures changes won't cause target rebuild(update)
# 因为每次 make 文件夹的时间会变如果不用 “|” 那么每一次都会完全重新编译

# things behind a like "d, lms" is settings for .lis file
# after "=" is the .lis file
# .lst file contains rich info about stack frame
$(BUILD_DIR)/%.o: %.c Makefile | $(BUILD_DIR)
	$(CC) -c $(CFLAGS) -Wa,-a,-ad,-alms=$(BUILD_DIR)/$(notdir $(<:%.c=%.lst)) $< -o $@
	
$(BUILD_DIR)/%.o: %.s Makefile | $(BUILD_DIR)
	$(AS) -c $(ASFLAGS) $< -o $@

$(BUILD_DIR)/$(TARGET).elf: $(OBJ) Makefile | $(BUILD_DIR)
	$(CC) $(OBJ) $(LDFLAG) -o $@
	$(SZ) $@

$(BUILD_DIR)/%.hex: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	$(CP) -O ihex $< $@

$(BUILD_DIR)/%.bin: $(BUILD_DIR)/%.elf | $(BUILD_DIR)
	$(CP) -O binary -S  $< $@

$(BUILD_DIR):
	mkdir $@

#######################################
# clean up
#######################################
clean:
	-rm -fR $(BUILD_DIR)

rebuild:
	make clean
	bear --append -- make

burn: all
	openocd -f interface/stlink-v2.cfg -f target/stm32f4x.cfg -c "program $(BUILD_DIR)/$(TARGET).hex verify reset exit"
#######################################
# dependencies
#######################################
-include $(wildcard $(BUILD_DIR)/*.d)

