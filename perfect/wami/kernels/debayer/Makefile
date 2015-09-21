TARGET := debayer
CFLAGS := -Iinc -DINPUT_SIZE=INPUT_SIZE_SMALL -DAUTOTUNER
LDFLAGS := -lm

ifneq ($(shell uname -s),Darwin)
	LDFLAGS += -lrt
endif

CLEANMETOO += err.txt out.txt small_kernel1_output.bin *.out *.log

RUNARGS := .

ACCEPT_DIR ?= ../../accept
include $(ACCEPT_DIR)/accept.mk
