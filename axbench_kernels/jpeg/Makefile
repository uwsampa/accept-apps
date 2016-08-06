TARGET := jpeg
LDFLAGS := -lm
CFLAGS :=

SOURCES := jpeg.c dct.c encoder.c huffman.c marker.c quant.c rgbimage.c

RUNARGS := baboon.dat out.jpeg

CLEANMETOO := $(TARGET).orig.ll out.jpeg *.bc inject_config.txt *.out *.log *.pyc

ACCEPT_DIR ?= ../../accept
include $(ACCEPT_DIR)/accept.mk
