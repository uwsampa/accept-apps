TARGET := fft-2d
# small input size is highly recommended; even the medium input
# takes more than 20 minutes to check in octave
CFLAGS := -Iinc -DINPUT_SIZE=INPUT_SIZE_SMALL
LDFLAGS := -lm -lrt

ACCEPT_DIR ?= /sampa/share/accept/accept

include $(ACCEPT_DIR)/accept.mk
