TARGET := debayer
CFLAGS := -Iinc -DINPUT_SIZE=INPUT_SIZE_LARGE
LDFLAGS := -lm -lrt

CLEANMETOO += err.txt large_kernel1_output.bin

RUNARGS := inout

ACCEPT_DIR ?= /sampa/share/accept/accept

include $(ACCEPT_DIR)/accept.mk
