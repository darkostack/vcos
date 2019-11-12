SRC_CORE_CPP += $(addprefix core/,\
	api/instance_api.cpp \
    api/stdiobase_api.cpp \
    api/thread_api.cpp \
    common/instance.cpp \
    common/thread.cpp \
    common/kernel_init.cpp \
    common/mutex.cpp \
    common/msg.cpp \
    utils/isrpipe.cpp \
    )

SRC_CORE_C += $(addprefix core/,\
    common/panic.c \
    utils/bitarithm.c \
    )

SRC_CLI_CPP += $(addprefix cli/,\
    cli.cpp \
    cli_uart.cpp \
    )

OBJS_SRC_CORE += $(addprefix $(BUILD)/, $(SRC_CORE_CPP:.cpp=.o))
OBJS_SRC_CORE += $(addprefix $(BUILD)/, $(SRC_CORE_C:.c=.o))

OBJS_SRC_CLI += $(addprefix $(BUILD)/, $(SRC_CLI_CPP:.cpp=.o))

OBJS_DIR += $(sort $(dir $(OBJS_SRC_CORE)))
OBJS_DIR += $(sort $(dir $(OBJS_SRC_CLI)))

LIB_SRC_CORE_ARCHIVE = $(BUILD)/$(LIB_SRC_CORE)
LIB_SRC_CORE_OBJS += $(OBJS_SRC_CORE)

LIB_SRC_CLI_ARCHIVE = $(BUILD)/$(LIB_SRC_CLI)
LIB_SRC_CLI_OBJS += $(OBJS_SRC_CLI)

ARCHIVE_FILES += $(LIB_SRC_CORE_ARCHIVE)
ARCHIVE_FILES += $(LIB_SRC_CLI_ARCHIVE)

all: | $(OBJS_DIR) $(ARCHIVE_FILES)

$(OBJS_DIR):
	$(MKDIR) -p $@

$(BUILD)/%.o: %.c
	$(ECHO) "CC $<"
	$(CC) -DFILE_RELATIVE=\"$(patsubst $(TOP)/%,%,$(abspath $<))\" \
          -DFILE_NOPATH=\"$(notdir $<)\" \
          $(CFLAGS) -c -o $@ $<

$(BUILD)/%.o: %.cpp
	$(ECHO) "CPP $<"
	$(CC) -DFILE_RELATIVE=\"$(patsubst $(TOP)/%,%,$(abspath $<))\" \
          -DFILE_NOPATH=\"$(notdir $<)\" \
          $(CPPFLAGS) -c -o $@ $<

$(BUILD)/%.o: %.s
	$(ECHO) "AS $<"
	$(CC) $(ASFLAGS) -c -o $@ $<

$(BUILD)/%.a:
	$(ECHO) "AR $@"
	$(AR) -cr $@ $^

$(LIB_SRC_CORE_ARCHIVE): $(LIB_SRC_CORE_OBJS)

$(LIB_SRC_CLI_ARCHIVE): $(LIB_SRC_CLI_OBJS)

-include $(OBJS_SRC_CORE:%.o=%.d)
-include $(OBJS_SRC_CLI:%.o=%.d)
