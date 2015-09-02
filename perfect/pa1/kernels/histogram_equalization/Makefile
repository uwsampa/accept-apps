TARGET := histogram_equalization
CFLAGS := -Iinc -DINPUT_SIZE=AUTOTUNER
LDFLAGS := -lm

CLEANMETOO += histeq_output.0.mat *.out *.log

ACCEPT_DIR ?= ../../accept
include $(ACCEPT_DIR)/accept.mk
