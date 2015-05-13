TARGET := blackscholes
LDFLAGS := -lm -pthread
CFLAGS := -pthread -DENABLE_THREADS

SOURCES := blackscholes.m4.cpp

RUNARGS := 1 in_64K.txt output.txt  # simlarge
TESTARGS := 1 in_10M.txt output.txt  # native
# RUNARGS := 4 in_1M.txt output.txt  # in between

CLEANMETOO := output.txt blackscholes.m4.cpp

ACCEPT_DIR := ../../accept
include $(ACCEPT_DIR)/accept.mk

RUNSHIM := pin -t ~/research/approx-limit/profiling/pin/pintools/full/obj-intel64/phase-profile.so -ldstsize 1 -bp 1 -cache1 1 -c1 32 -a1 4 -b1 32 -cache2 0 -opcodes 0 -phases 0 -reg 1 -- 

#-phases 1 -i ~/research/accept-apps/blackscholes/phases.cfg -reg 1 -- 

blackscholes.m4.cpp: blackscholes.c c.m4.pthreads
	m4 c.m4.pthreads $< > $@
