TARGET := blackscholes
LDFLAGS := -lm -pthread
CFLAGS := -pthread -DENABLE_THREADS

SOURCES := blackscholes.m4.cpp

RUNARGS := 1 in_64K.txt out.txt  # simlarge
TESTARGS := 1 in_10M.txt out.txt  # native

CLEANMETOO := out.txt blackscholes.m4.cpp *.bc inject_config.txt *.out *.log

ACCEPT_DIR ?= ../../accept
include $(ACCEPT_DIR)/accept.mk

blackscholes.m4.cpp: blackscholes.c c.m4.pthreads
	m4 c.m4.pthreads $< > $@
