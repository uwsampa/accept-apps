TARGET := fft-1d
CFLAGS := -Iinc -DINPUT_SIZE=INPUT_SIZE_SMALL -DAUTOTUNER -DFIXED_POINT_LENGTH=16 -DSHIFT_OR_SATURATE=0
#CFLAGS := -Iinc -DINPUT_SIZE=INPUT_SIZE_SMALL -DAUTOTUNER -DFIXED_POINT_LENGTH=16
#CFLAGS := -Iinc -DINPUT_SIZE=INPUT_SIZE_SMALL -DAUTOTUNER 
LDFLAGS := -lm

ifneq ($(shell uname -s),Darwin)
	LDFLAGS += -lrt
endif

CLEANMETOO := $(TARGET).orig.ll *.bc inject_config.txt random_input.mat fft_output.mat out.mat *.out *.log *.pyc

ACCEPT_DIR ?= ~/Codes/GitHub/accept
include $(ACCEPT_DIR)/accept.mk
