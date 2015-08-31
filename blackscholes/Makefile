TARGET := blackscholes
LDFLAGS := -lm -pthread
CFLAGS := -pthread -DENABLE_THREADS

SOURCES := blackscholes.m4.cpp

RUNARGS := 1 in_64K.txt output.txt  # simlarge
TESTARGS := 1 in_10M.txt output.txt  # native

CLEANMETOO := output.txt blackscholes.m4.cpp *.bc inject_config.txt

ACCEPT_DIR ?= ../../accept
include $(ACCEPT_DIR)/accept.mk

blackscholes.m4.cpp: blackscholes.c c.m4.pthreads
	m4 c.m4.pthreads $< > $@
