PLAT := $(realpath $(dir $(lastword $(MAKEFILE_LIST))))

ZYNQDIR := $(PLAT)/zynqlib
override CFLAGS += -target arm-none-linux-gnueabi \
	-ccc-gcc-name arm-linux-gnueabi-gcc \
	-D_GNU_SOURCE=1 \
	-I$(ZYNQDIR) -I$(ZYNQDIR)/bsp/include
ARMTOOLCHAIN ?= /sampa/share/Xilinx/14.6/14.6/ISE_DS/EDK/gnu/arm/lin
LINKER := $(ARMTOOLCHAIN)/bin/arm-xilinx-eabi-gcc
LDFLAGS := -Wl,-T -Wl,$(ZYNQDIR)/lscript.ld -L$(ZYNQDIR)/bsp/lib
LIBS := -Wl,--start-group,-lxil,-lgcc,-lc,-lm,--end-group
LLCARGS += -march=arm -mcpu=cortex-a9
RUNSHIM := $(PLAT)/zynqrun.sh $(ZYNQBIT)
CLEANMETOO += output.txt zynqlog.txt
OPTARGS += -accept-npu
