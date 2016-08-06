TARGET := kmeans
LDFLAGS := -lm
CFLAGS :=

SOURCES := kmeans.c distance.c rgbimage.c segmentation.c

RUNARGS := baboon.dat out.dat

CLEANMETOO := $(TARGET).orig.ll out.dat *.bc inject_config.txt *.out *.log *.pyc

ACCEPT_DIR ?= ../../accept
include $(ACCEPT_DIR)/accept.mk
