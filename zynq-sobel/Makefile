ARCH := zynq
TARGET := sobel
ZYNQBIT := sobel_x8_x64.bit
LDFLAGS := -lm
OPTARGS += -accept-npu-bufsize=1152

RUNARGS := baboon.dat
TESTARGS := lena.dat

include ../plat/zynq.mk
ACCEPT_DIR := ../../accept
include $(ACCEPT_DIR)/accept.mk
