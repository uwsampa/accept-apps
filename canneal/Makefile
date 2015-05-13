TARGET := canneal
CFLAGS := -DENABLE_THREADS -pthread
LDFLAGS := -pthread

HEADERS := $(wildcard *.h)

TESTARGS := 1 15000 2000 2500000.nets 6000  # native
RUNARGS := 1 15000 2000 400000.nets 128  # simlarge
#RUNARGS := 4 15000 2000 200000.nets 64  # simmedium
# RUNARGS := 1 10000 2000 100000.nets 32  # simsmall
# RUNARGS := 1 100 300 100.nets 2  # simdev

RUNSHIM := pin -t ~/research/approx-limit/profiling/pin/pintools/full/obj-intel64/phase-profile.so -ldstsize 1 -bp 1 -cache1 1 -c1 32 -a1 4 -b1 32 -cache2 0 -opcodes 0 -phases 0 -reg 1 -- 

CLEANMETOO := output.txt

ACCEPT_DIR := ../../accept
include $(ACCEPT_DIR)/accept.mk
