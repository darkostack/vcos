include .gitinfo.mk

# Set verbosity
ifeq ($(v),1)
Q =
else
Q = @
endif

# Optimization level
OPT ?= -Os

HOST_MACHINE = $(shell uname -s)
CROSS_COMPILE ?= arm-none-eabi-

export CC = $(Q)$(CROSS_COMPILE)gcc
export AS = $(Q)$(CROSS_COMPILE)gcc
export LD = $(Q)$(CROSS_COMPILE)ld
export AR = $(Q)$(CROSS_COMPILE)ar
export CPP = $(Q)$(CROSS_COMPILE)g++
export SIZE = $(Q)$(CROSS_COMPILE)size
export STRIP = $(Q)$(CROSS_COMPILE)strip -s
export OBJCOPY = $(Q)$(CROSS_COMPILE)objcopy
export OBJDUMP = $(Q)$(CROSS_COMPILE)objdump
export RM = $(Q)rm
export MKDIR = $(Q)mkdir
export ECHO = $(Q)@echo
ifeq ($(v),1)
export MAKE = $(Q)make
else
export MAKE = $(Q)make -s
endif
export TOP = $(shell pwd)

VCOS = vcos
PLATFORM ?= sirius
APPS ?= cli

BUILD = $(TOP)/build/$(PLATFORM)
IMAGE = $(BUILD)/vcos-$(APPS)

INC += -I$(TOP)
INC += -I$(TOP)/include
INC += -I$(TOP)/platform
INC += -I$(TOP)/src
INC += -I$(TOP)/src/core

DEF += -DVCOS_PLATFORM_$(shell echo $(PLATFORM) | tr a-z A-Z)
DEF += -DVCOS_ENABLE_$(shell echo $(APPS) | tr a-z A-Z)

DEF += -DPACKAGE_NAME='\"VCOS\"'
DEF += -DPACKAGE_VERSION='\"$(GIT_REPO_COMMIT)\"'

include $(TOP)/platform/$(PLATFORM)/platform.mk
include $(TOP)/apps/$(APPS)/apps.mk

CFLAGS += -fdata-sections -ffunction-sections
CFLAGS += $(OPT)
CFLAGS += -Wno-implicit-fallthrough -Wno-unused-parameter
CFLAGS += $(PLATFORM_FLAGS)
CFLAGS += -Wall -Werror
CFLAGS += -std=c99
CFLAGS += -D_BSD_SOURCE=1 -D_DEFAULT_SOURCE=1
CFLAGS += $(DEF)
CFLAGS += $(INC)

CPPFLAGS += -fdata-sections -ffunction-sections
CPPFLAGS += $(OPT)
CPPFLAGS += -Wno-implicit-fallthrough -Wno-unused-parameter
CPPFLAGS += $(PLATFORM_FLAGS)
CPPFLAGS += -fno-exceptions -fno-rtti
CPPFLAGS += -Wall -Werror
CPPFLAGS += -std=gnu++98 -Wno-c++14-compat
CPPFLAGS += -D_BSD_SOURCE=1 -D_DEFAULT_SOURCE=1
CPPFLAGS += $(DEF)
CPPFLAGS += $(INC)

ASFLAGS += -fdata-sections -ffunction-sections
ASFLAGS += $(OPT)
ASFLAGS += -Wno-implicit-fallthrough -Wno-unused-parameter
ASFLAGS += $(PLATFORM_FLAGS)

LDFLAGS += -fdata-sections -ffunction-sections
LDFLAGS += $(OPT) 
LDFLAGS += -Wno-implicit-fallthrough -Wno-unused-parameter
LDFLAGS += $(PLATFORM_FLAGS)
LDFLAGS += -specs=nano.specs
LDFLAGS += -specs=nosys.specs
LDFLAGS += -Wl,--gc-sections

export LIB_PLATFORM = libplatform.a
export LIB_SRC_CORE = libsrc_core.a
export LIB_SRC_CLI = libsrc_cli.a
export LIB_APPS = libapps.a

LIBS += $(BUILD)/platform/$(LIB_PLATFORM)
LIBS += $(BUILD)/src/$(LIB_SRC_CORE)
LIBS += $(BUILD)/src/$(LIB_SRC_CLI)
LIBS += $(BUILD)/apps/$(LIB_APPS)

LDSCRIPT = $(TOP)/platform/$(PLATFORM)/$(PLATFORM).ld

all: $(VCOS) BUILD_FINISHED_INFO

$(BUILD):
	$(MKDIR) -p $@

OBJS:
	$(MAKE) -C platform BUILD="$(BUILD)/platform" CFLAGS="$(CFLAGS)" ASFLAGS="$(ASFLAGS)"
	$(MAKE) -C src BUILD="$(BUILD)/src" CFLAGS="$(CFLAGS)" CPPFLAGS="$(CPPFLAGS)"
	$(MAKE) -C apps BUILD="$(BUILD)/apps" CFLAGS="$(CFLAGS)" CPPFLAGS="$(CPPFLAGS)"

$(IMAGE).elf: $(LIBS)
	$(CC) $(LDFLAGS) $(patsubst %,-L%,$(patsubst %/,%,$(sort $(dir $(LIBS)))))  \
	-Wl,--whole-archive $(patsubst %,-l%,$(patsubst lib%,%,$(sort $(basename $(notdir $(LIBS)))))) \
	-Wl,--no-whole-archive -T $(LDSCRIPT) \
	-Wl,-Map,$(BUILD)/vcos-$(APPS).map -o $@

$(IMAGE).bin: $(IMAGE).elf
	$(OBJCOPY) -O binary $< $@

$(IMAGE).lst: $(IMAGE).elf
	$(OBJDUMP) -h -S $< > $@

$(VCOS): OBJS $(IMAGE).bin $(IMAGE).lst | $(BUILD)
	$(SIZE) --format=berkeley $(IMAGE).elf

BUILD_FINISHED_INFO:
	$(ECHO) "---------------------------------------------------------------------"
	$(ECHO) "\033[32mFinished Building\033[0m : [$(PLATFORM)] in $(HOST_MACHINE)"
	$(ECHO) "---------------------------------------------------------------------"
	$(ECHO) "Image            : build/$(PLATFORM)/\033[32mvcos-$(APPS).bin\033[0m"
	$(ECHO) "---------------------------------------------------------------------"
	$(ECHO) "Build Start Time : $(shell date +%b\ %0d\ %Y\ %0H:%0M:%0S)"
	$(ECHO) "Build End Time   : $(shell date +%b\ %0d\ %Y\ %0H:%0M:%0S)"
	$(ECHO) "---------------------------------------------------------------------"
	$(ECHO) "Repository info: $(GIT_REPO_COMMIT) @ [\033[33m$(GIT_REPO_BRANCH)\033[0m], dirty = $(GIT_REPO_DIRTY)"

.PHONY: all clean size

# Note: `make clean` will remove everything inside `build` folder
clean:
	-rm -rf $(BUILD)

size:
	$(SIZE) --format=SysV $(IMAGE).elf
