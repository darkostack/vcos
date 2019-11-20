APPS_ARDUINO_CPP += main.cpp

OBJS_APPS_ARDUINO += $(addprefix $(BUILD)/, $(APPS_ARDUINO_CPP:.cpp=.o))

OBJS_DIR += $(sort $(dir $(OBJS_APPS_ARDUINO)))

LIB_APPS_ARCHIVE = $(BUILD)/$(LIB_APPS)
LIB_APPS_OBJS += $(OBJS_APPS_ARDUINO)

ARCHIVE_FILES += $(LIB_APPS_ARCHIVE)

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

$(LIB_APPS_ARCHIVE): $(LIB_APPS_OBJS)

-include $(OBJS_APPS_ARDUINO:%.o=%.d)
