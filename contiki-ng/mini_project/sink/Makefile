CONTIKI_PROJECT = sink
all: $(CONTIKI_PROJECT)
WERROR := 1

CFLAGS += -O3
# CFLAGS += -Os  # optimize for size

CUSTOM_CFLAGS := -Wall
CUSTOM_CFLAGS += -Wformat
CUSTOM_CFLAGS += -std=c11

define CUSTOM_RULE_C_TO_O
endef

define CUSTOM_RULE_CPP_TO_O
endef

define CUSTOM_RULE_C_TO_S
endef

define CUSTOM_RULE_C_TO_E
endef

define CUSTOM_RULE_CPP_TO_E
endef


CONTIKI = ../..
include $(CONTIKI)/Makefile.include

CUSTOM_CFLAGS += $(filter-out -Wno-unused-const-variable,$(CFLAGS))

%.o: %.c
	@echo "Using custom rule for project .c files"
	$(TRACE_CC)
	$(Q)$(CC) $(CUSTOM_CFLAGS) -c $< -o $@


%.o: %.cpp
	@echo "Using custom rule for project .cpp files"
	$(TRACE_CXX)
	$(Q)$(CXX) $(CXXFLAGS) -c $< -o $@


%.s: %.c
	$(TRACE_CC)
	$(Q)$(CC) $(CFLAGS) -S $< -o $@


%.e: %.c
	$(TRACE_CC)
	$(Q)$(CC) $(CFLAGS) -E $< -o $@


%.e: %.cpp
	$(TRACE_CXX)
	$(Q)$(CXX) $(CXXFLAGS) -E $< -o $@