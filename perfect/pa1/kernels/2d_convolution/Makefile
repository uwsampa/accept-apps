TARGET := 2d_convolution
CFLAGS := -Iinc -DINPUT_SIZE=INPUT_SIZE_SMALL -DAUTOTUNER
LDFLAGS := -lm

ifneq ($(shell uname -s),Darwin)
	LDFLAGS += -lrt
endif

CLEANMETOO += $(TARGET).orig.ll 2dconv_output.0.mat out.mat *.out *.log *.pyc

ACCEPT_DIR ?= ~/Codes/GitHub/accept 
include $(ACCEPT_DIR)/accept.mk
