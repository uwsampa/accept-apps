TARGET := 2d_convolution
CFLAGS := -Iinc -DINPUT_SIZE=INPUT_SIZE_SMALL -DAUTOTUNER
LDFLAGS := -lm

ifneq ($(shell uname -s),Darwin)
	LDFLAGS += -lrt
endif

CLEANMETOO += 2dconv_output.0.mat *.out *.log

ACCEPT_DIR ?= ../../accept
include $(ACCEPT_DIR)/accept.mk
