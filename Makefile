LOCAL_DIR := $(abspath $(dir $(word $(words $(MAKEFILE_LIST)),$(MAKEFILE_LIST))))
BUILDROOT := $(LOCAL_DIR)/out

HIDE := @
ifeq ($(V),1)
	HIDE :=
endif

MKDIR = if [ ! -d $(dir $@) ]; then mkdir -p $(dir $@); fi
VERSION := $(shell git rev-parse HEAD | cut -c1-8)
DEFINE := STM32F10X_HD

TOOLCHAIN_PREFIX := arm-none-eabi-
CCACHE ?=
CC := $(CCACHE) $(TOOLCHAIN_PREFIX)gcc
LD := $(TOOLCHAIN_PREFIX)ld
OBJDUMP := $(TOOLCHAIN_PREFIX)objdump
OBJCOPY := $(TOOLCHAIN_PREFIX)objcopy
CPPFILT := $(TOOLCHAIN_PREFIX)c++filt
SIZE := $(TOOLCHAIN_PREFIX)size

ARCHFLAGS := -mcpu=cortex-m3 -mfloat-abi=soft -mthumb
CFLAGS := -Os -g --std=gnu11
CFLAGS += -ffunction-sections -fdata-sections -fno-common -finline
CFLAGS += -Wall -Wno-sign-compare -Wno-unused-parameter -Werror=return-type -Werror-implicit-function-declaration
INCDIR := -I$(LOCAL_DIR)/user/inc
INCDIR += -I$(LOCAL_DIR)/cmsis/CMSIS/Core/Include
INCDIR += -I$(LOCAL_DIR)/cmsis/CMSIS/Driver/Include
INCDIR += -I$(LOCAL_DIR)/cmsis/CMSIS/Driver/Source
INCDIR += -I$(LOCAL_DIR)/cmsis/Device/STM32F10xx/Include
INCDIR += -I$(LOCAL_DIR)/cmsis/Device/STM32F10xx/StdPeriph_Driver/inc
CFLAGS_VERSION := -DVERSION_NUM="\"$(VERSION)\""

LKSCRIPT := $(LOCAL_DIR)/ioboard.ld
LDFLAGS := --cref -dT $(LKSCRIPT) --gc-sections
LDFLAGS += -Map $(BUILDROOT)/ioboard.map
DEFINE_FLAGS := $(addprefix -D,$(DEFINE))

# find the direct path to libgcc.a for our particular multilib variant
LIBGCC := $(shell $(TOOLCHAIN_PREFIX)gcc $(ARCHFLAGS) -print-file-name=libm.a)
LIBGCC += $(shell $(TOOLCHAIN_PREFIX)gcc $(ARCHFLAGS) -print-file-name=libgcc.a)
LIBGCC += $(shell $(TOOLCHAIN_PREFIX)gcc $(ARCHFLAGS) -print-file-name=libc_nano.a)

USER_SRC = \
	$(LOCAL_DIR)/user/main.c \
	$(LOCAL_DIR)/user/sys.c \
	$(LOCAL_DIR)/user/startup.c \
	$(LOCAL_DIR)/user/peripheral.c \
	$(LOCAL_DIR)/user/utils.c \
	$(LOCAL_DIR)/user/ppm_parse.c \

CMSIS_SRC = \
	$(LOCAL_DIR)/cmsis/CMSIS/Driver/Source/USART_STM32F10x.c \
	$(LOCAL_DIR)/cmsis/CMSIS/Driver/Source/GPIO_STM32F10x.c \
	$(LOCAL_DIR)/cmsis/CMSIS/Driver/Source/DMA_STM32F10x.c \

STD_SRC = \
	$(LOCAL_DIR)/cmsis/Device/STM32F10xx/Source/system_stm32f10x.c \
	$(LOCAL_DIR)/cmsis/Device/STM32F10xx/StdPeriph_Driver/src/stm32f10x_adc.c \
	$(LOCAL_DIR)/cmsis/Device/STM32F10xx/StdPeriph_Driver/src/stm32f10x_rcc.c \
	$(LOCAL_DIR)/cmsis/Device/STM32F10xx/StdPeriph_Driver/src/stm32f10x_gpio.c \
	$(LOCAL_DIR)/cmsis/Device/STM32F10xx/StdPeriph_Driver/src/stm32f10x_tim.c \
	$(LOCAL_DIR)/cmsis/Device/STM32F10xx/StdPeriph_Driver/src/misc.c \

SRC = $(USER_SRC) \
	$(STD_SRC) \
	$(LOCAL_DIR)/newlib_stub.c \
	$(CMSIS_SRC) \

MKFILE := $(lastword $(MAKEFILE_LIST))
HOBJ := $(patsubst $(LOCAL_DIR)/%.S, $(BUILDROOT)/%.o, $(HSRC))
OBJ := $(patsubst $(LOCAL_DIR)/%.c, $(BUILDROOT)/%.o, $(SRC))
DEP := $(patsubst $(LOCAL_DIR)/%.c, $(BUILDROOT)/%.d, $(SRC))
BLELF := $(BUILDROOT)/ioboard.elf
BLHEX := $(BUILDROOT)/ioboard.hex
BLBIN := $(BUILDROOT)/ioboard.bin

all: ioboard
.PHONY: all ioboard

ioboard: $(BLHEX) $(BLBIN)

$(BLBIN): $(BLELF)
	@echo generating image: $@
	$(HIDE) $(OBJCOPY) -O binary $< $@

$(BLHEX): $(BLELF)
	@echo generating image: $@
	$(HIDE) $(OBJCOPY) -O ihex $< $@

$(BLELF): $(OBJ) $(LKSCRIPT) $(PRE)
	@echo linking $@
	$(HIDE) $(LD) $(LDFLAGS) $(OBJ) $(LIBGCC) -o $@
	$(HIDE) $(SIZE) $@
	@echo generating sorted symbols: $@.sym
	$(HIDE) $(OBJDUMP) -t $@ | $(CPPFILT) | sort > $@.sym
	@echo generating listing: $@.lst
	$(HIDE) $(OBJDUMP) -Mreg-names-raw -d $@ | $(CPPFILT) > $@.lst

$(OBJ): $(BUILDROOT)/%.o: $(LOCAL_DIR)/%.c $(MKFILE)
	$(HIDE) $(MKDIR)
	@echo compiling $(patsubst $(LOCAL_DIR)/%.c,%.c,$<)
	$(HIDE) $(CC) $(ARCHFLAGS) $(CFLAGS) $(CFLAGS_VERSION) $(INCDIR) $(DEFINE_FLAGS) -c $< -MD -MP -MT $@ -o $@

$(PRE): $(BUILDROOT)/%.i: $(LOCAL_DIR)/%.c $(MKFILE)
	$(HIDE) $(MKDIR)
	@echo compiling $(patsubst $(LOCAL_DIR)/%.c,%.c,$<)
	$(HIDE) $(CC) $(ARCHFLAGS) $(CFLAGS) $(CFLAGS_VERSION) $(INCDIR) $(DEFINE_FLAGS) -E $< -MD -MP -MT $@ -o $@

-include $(DEP)

clean: ioboard-clean
.PHONY: clean ioboard-clean

ioboard-clean:
	$(HIDE) rm -f $(OBJ) $(HOBJ) $(DEP) $(BLELF) $(BLHEX) $(BLELF).sym $(BLELF).lst $(BUILDROOT)/ioboard.map
