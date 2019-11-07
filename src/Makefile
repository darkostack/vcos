SRC_CORE_CPP += $(addprefix core/,\
    common/instance.cpp \
	api/instance_api.cpp \
    )

OBJS_SRC_CORE += $(addprefix $(BUILD)/, $(SRC_CORE_CPP:.cpp=.o))

OBJS_DIR += $(sort $(dir $(OBJS_SRC_CORE)))

LIB_SRC_CORE_ARCHIVE = $(BUILD)/$(LIB_SRC_CORE)
LIB_SRC_CORE_OBJS += $(OBJS_SRC_CORE)

ARCHIVE_FILES += $(LIB_SRC_CORE_ARCHIVE)

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

-include $(OBJS_SRC_CORE:%.o=%.d)
