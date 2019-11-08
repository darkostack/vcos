PLATFORM_SIRIUS_C += $(addprefix sirius/,\
    cortexm/cortexm_init.c \
    cortexm/irq.c \
    cortexm/vectors_cortexm.c \
    periph/gpio.c \
    periph/uart.c \
    assert.c \
    cpu_init.c \
    stdio_uart.c \
    syscalls.c \
    vectors.c \
    )

OBJS_PLATFORM_SIRIUS += $(addprefix $(BUILD)/, $(PLATFORM_SIRIUS_C:.c=.o))

OBJS_DIR += $(sort $(dir $(OBJS_PLATFORM_SIRIUS)))

LIB_PLATFORM_ARCHIVE = $(BUILD)/$(LIB_PLATFORM)
LIB_PLATFORM_OBJS += $(OBJS_PLATFORM_SIRIUS)

ARCHIVE_FILES += $(LIB_PLATFORM_ARCHIVE)

all: | $(OBJS_DIR) $(ARCHIVE_FILES)

$(OBJS_DIR):
	$(MKDIR) -p $@

$(BUILD)/%.o: %.c
	$(ECHO) "CC $<"
	$(CC) -DFILE_RELATIVE=\"$(patsubst $(TOP)/%,%,$(abspath $<))\" \
          -DFILE_NOPATH=\"$(notdir $<)\" \
          $(CFLAGS) -c -o $@ $<

$(BUILD)/%.o: %.s
	$(ECHO) "AS $<"
	$(CC) $(ASFLAGS) -c -o $@ $<

$(BUILD)/%.a:
	$(ECHO) "AR $@"
	$(AR) -cr $@ $^

$(LIB_PLATFORM_ARCHIVE): $(LIB_PLATFORM_OBJS)

-include $(OBJS_PLATFORM_SIRIUS:%.o=%.d)
