TARGET := jpeg
LDFLAGS := -lm
RUNARGS := baboon.rgb out.jpg

CLEANMETOO := out.jpg baboon.rgb.jpg data/baboon-220px.rgb.jpg *.out *.log

SOURCES := jpeg.c huffman.c encoder.c dct.c marker.c quant.c rgbimage.c

ACCEPT_DIR ?= ../../accept
include $(ACCEPT_DIR)/accept.mk
