TARGET := fft-1d
CFLAGS := -Iinc -DINPUT_SIZE=INPUT_SIZE_LARGE
LDFLAGS := -lm -lrt

CLEANMETOO := *.bc inject_config.txt

#cc -o fft-1d 
#   -Iinc -I../../../lib -DINPUT_SIZE=INPUT_SIZE_LARGE
#   -std=c89 -pedantic -ansi -Wall -g -O3
#   src/main-fft-1d.o src/fft-fft-1d.o src/octave-fft-1d.o
#   ../../../lib/libtiming.a ../../../lib/libxmem.a
#   -lm -lrt

ACCEPT_DIR ?= /sampa/share/accept/accept

include $(ACCEPT_DIR)/accept.mk
