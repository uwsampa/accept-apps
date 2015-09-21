TARGET := lucas-kanade
CFLAGS := -Iinc -DINPUT_SIZE=INPUT_SIZE_SMALL -DAUTOTUNER
LDFLAGS := -lm

ifneq ($(shell uname -s),Darwin)
	LDFLAGS += -lrt
endif

CLEANMETOO += output.mat out.mat *.out *.log

ACCEPT_DIR ?= ../../accept
include $(ACCEPT_DIR)/accept.mk
