TARGET := jmeint
LDFLAGS := -lm
CFLAGS :=

SOURCES := jmeint.c tritri.c

RUNARGS := jmeint_10K.data out.data

CLEANMETOO := $(TARGET).orig.ll out.data *.bc inject_config.txt *.out *.log *.pyc

ACCEPT_DIR ?= ../../accept
include $(ACCEPT_DIR)/accept.mk
