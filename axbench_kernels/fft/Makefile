TARGET := fft
LDFLAGS := -lm
CFLAGS :=

SOURCES := fft.c fourier.c complex.c

RUNARGS := 2048 out.data

CLEANMETOO := $(TARGET).orig.ll out.data *.bc inject_config.txt *.out *.log *.pyc

ACCEPT_DIR ?= ../../accept
include $(ACCEPT_DIR)/accept.mk
