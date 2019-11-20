INC += -I$(TOP)/platform/sirius
INC += -I$(TOP)/platform/sirius/cortexm
INC += -I$(TOP)/platform/sirius/cortexm/cmsis

DEF += -DVCOS_PROJECT_CORE_CONFIG_FILE='\"vcos-core-sirius-config.h\"'

PLATFORM_FLAGS += -mcpu=cortex-m3 -mlittle-endian -mthumb -mfloat-abi=soft
PLATFORM_FLAGS += -march=armv7-m -mno-unaligned-access

PLATFORM_LDSCRIPT = $(TOP)/platform/sirius/sirius.ld
