TARGET := fft-1d
CFLAGS := -Iinc -DINPUT_SIZE=INPUT_SIZE_SMALL -DAUTOTUNER
LDFLAGS := -lm

ifneq ($(shell uname -s),Darwin)
	LDFLAGS += -lrt
endif

CLEANMETOO := *.bc inject_config.txt random_input.mat fft_output.mat out.mat *.out *.log

ACCEPT_DIR ?= ../../accept
include $(ACCEPT_DIR)/accept.mk
