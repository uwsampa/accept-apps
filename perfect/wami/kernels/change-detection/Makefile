TARGET := change-detection
CFLAGS := -Iinc -DINPUT_SIZE=INPUT_SIZE_SMALL -DAUTOTUNER
LDFLAGS := -lm

ifneq ($(shell uname -s),Darwin)
	LDFLAGS += -lrt
endif

CLEANMETOO += err.txt out.txt small_kernel3_output.bin *.out *.log

RUNARGS := .

ACCEPT_DIR ?= ../../accept
include $(ACCEPT_DIR)/accept.mk
