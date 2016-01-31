TARGET := pfa_interp1
CFLAGS := -Iinc -DINPUT_SIZE=INPUT_SIZE_SMALL -DAUTOTUNER
LDFLAGS := -lm

ifneq ($(shell uname -s),Darwin)
	LDFLAGS += -lrt
endif

CLEANMETOO += snr.txt small_kernel1_output.bin out.bin *.out *.log *.pyc

RUNARGS := .

ACCEPT_DIR ?= ../../accept
include $(ACCEPT_DIR)/accept.mk