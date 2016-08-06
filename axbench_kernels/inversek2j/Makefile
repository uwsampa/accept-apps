TARGET := inversek2j
LDFLAGS := -lm
CFLAGS :=

SOURCES := inversek2j.c kinematics.c

RUNARGS := theta_100K.data out.data

CLEANMETOO := $(TARGET).orig.ll out.data *.bc inject_config.txt *.out *.log *.pyc

ACCEPT_DIR ?= ../../accept
include $(ACCEPT_DIR)/accept.mk
