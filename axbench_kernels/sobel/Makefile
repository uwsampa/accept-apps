TARGET := sobel
LDFLAGS := -lm
CFLAGS :=

SOURCES := sobel.c convolution.c rgb_image.c

RUNARGS := baboon.dat out.dat

CLEANMETOO := $(TARGET).orig.ll out.dat *.bc inject_config.txt *.out *.log *.pyc

ACCEPT_DIR ?= ../../accept
include $(ACCEPT_DIR)/accept.mk
