TARGET := inner-product
CFLAGS := -Iinc -DINPUT_SIZE=INPUT_SIZE_SMALL -DAUTOTUNER
LDFLAGS := -lm

ifneq ($(shell uname -s),Darwin)
	LDFLAGS += -lrt
endif

CLEANMETOO += $(TARGET).orig.ll snr.txt out.bin *.out *.log *.pyc

RUNARGS := .

ACCEPT_DIR ?= ../../accept
include $(ACCEPT_DIR)/accept.mk
