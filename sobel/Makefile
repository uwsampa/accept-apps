
TARGET := sobel
CLEANMETOO := out.pgm *.bc inject_config.txt

LIBERROR_SRCS := liberror.cpp enerj.cpp flikker.cpp reducedprecfp.cpp lva.cpp npu.cpp

RUNARGS := i1.pgm
TESTARGS := i2.pgm

SOURCES := sobel.c
SOURCES += $(LIBERROR_SRCS)

#RUNSHIM := pin -t ~/research/approx-limit/profiling/pin/pintools/full/obj-intel64/phase-profile.so -ldstsize 1 -bp 1 -cache1 1 -c1 32 -a1 4 -b1 32 -cache2 0 -opcodes 0 -phases 0 -reg 1 -- 

#RUNSHIM := pin -t ~/research/approx-limit/profiling/pin/pintools/full/obj-intel64/phase-profile.so -ldstsize 1 -bp 1 -cache1 1 -c1 32 -a1 4 -b1 32 -cache2 0 -opcodes 0 -phases 1 -i ~/research/accept-apps/sobel/phases.cfg -reg 1 -- 

ACCEPT_DIR := ../../accept
include $(ACCEPT_DIR)/accept.mk
