ARCH := zynq
ZYNQBIT := inversek2j_x8_x64.bit
OPTARGS += -accept-npu-bufsize=64

RUNARGS := trainseed.txt
TESTARGS := testseed.txt

include ../plat/zynq.mk
ACCEPT_DIR := ../../accept
include $(ACCEPT_DIR)/accept.mk
