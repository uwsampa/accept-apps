TARGET := sobel
LDFLAGS := -lm
ifneq ($(shell uname -s),Darwin)
	LDFLAGS += -lrt
endif

CLEANMETOO := out.rgb *.png *.bc *.ll *.out *.log output.csv output.nn

RUNARGS := baboon.rgb
TESTARGS := lena.rbg

include $(ACCEPT_DIR)/accept.mk
