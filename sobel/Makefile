ACCEPT_DIR ?= /sampa/home/moreau/Desktop/research/accept
ACCEPTAPPS_DIR ?= /sampa/home/moreau/Desktop/research/accept-apps

TARGET := sobel
CLEANMETOO := out.pgm *.bc *.out *.log ../sobel_outputs ../sobel_tmp_outputs

LIBERROR_DIR = $(ACCEPTAPPS_DIR)/liberror
LIBERROR_FNAMES := liberror.cpp flikker.cpp lva.cpp npu.cpp overscaledalu.cpp reducedprecfp.cpp sram.cpp dram.cpp
LIBERROR_SRCS = $(LIBERROR_FNAMES:%.cpp=$(LIBERROR_DIR)/%.cpp)

RUNARGS := i1.pgm
TESTARGS := i2.pgm

SOURCES := sobel.c
SOURCES += $(LIBERROR_SRCS)

include $(ACCEPT_DIR)/accept.mk