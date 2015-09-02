TARGET := histogram_equalization
CFLAGS := -Iinc -DINPUT_SIZE=INPUT_SIZE_LARGE
LDFLAGS := -lm

CLEANMETOO += histeq_output.0.mat *.out *.log

ACCEPT_DIR ?= ../../accept
include $(ACCEPT_DIR)/accept.mk
