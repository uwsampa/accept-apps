TARGET := blackscholes
LDFLAGS := -lm -pthread
CFLAGS :=

SOURCES := blackscholes.c

RUNARGS := in_64K.txt out.txt  # simlarge

CLEANMETOO := $(TARGET).orig.ll out.txt *.bc inject_config.txt *.out *.log *.pyc

ACCEPT_DIR ?= ../../accept
include $(ACCEPT_DIR)/accept.mk
