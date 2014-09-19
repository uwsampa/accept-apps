ARCH := zynq
TARGET := sobel
ZYNQBIT := sobel_x8_x64.bit
LDFLAGS := -lm
OPTARGS += -accept-npu-bufsize=1152

RUNARGS := baboon.dat
TESTARGS := lena.dat

ACCEPT_DIR := ../../accept
include $(ACCEPT_DIR)/../accept-apps/plat/zynq.mk
include $(ACCEPT_DIR)/accept.mk
