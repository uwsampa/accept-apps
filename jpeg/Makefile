TARGET := jpeg
LDFLAGS := -lm
RUNARGS := baboon.rgb

CLEANMETOO := baboon.rgb.jpg data/baboon-220px.rgb.jpg

LIBERROR_SRCS := liberror.cpp enerj.cpp flikker.cpp reducedprecfp.cpp lva.cpp

SOURCES := jpeg.c huffman.c encoder.c dct.c marker.c quant.c rgbimage.c
SOURCES += $(LIBERROR_SRCS)

#RUNSHIM := pin -t ~/research/approx-limit/profiling/pin/pintools/full/obj-intel64/phase-profile.so -ldstsize 1 -bp 1 -cache1 1 -c1 32 -a1 4 -b1 32 -cache2 0 -opcodes 0 -phases 0 -reg 1 -- 

#RUNSHIM := pin -t ~/research/approx-limit/profiling/pin/pintools/full/obj-intel64/phase-profile.so -ldstsize 1 -bp 1 -cache1 1 -c1 32 -a1 4 -b1 32 -cache2 0 -opcodes 0 -phases 1 -i ~/research/accept-apps/jpeg/phases.cfg -reg 1 -- 

ACCEPT_DIR := ../../accept
include $(ACCEPT_DIR)/accept.mk
