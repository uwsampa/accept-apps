PLAT 	   := $(realpath $(dir $(lastword $(MAKEFILE_LIST))))

LINKER     := msp430-gcc
LLCARGS    += -march=msp430 -msp430-hwmult-mode=no
LDFLAGS    += -mmcu=msp430fr5969
RUNSHIM    := $(PLAT)/msp430/run.sh
MSP430LIBS += perfctr
MSP430DEPS := $(foreach L,$(MSP430LIBS),$(PLAT)/msp430rt/$(L)/lib$(L).a)
EXTRADEPS  += $(MSP430DEPS)
override \
CFLAGS     += -target msp430-elf -Wall -fno-stack-protector -D__MSP430FR5969__ \
		$(addprefix -I, \
			$(shell msp430-cpp -Wp,-v </dev/null 2>&1 | \
			grep /include | sed -e 's/^ *//')) \
		$(addprefix -I$(PLAT)/msp430rt/,$(MSP430LIBS))
LIBS       += $(foreach L,$(MSP430LIBS),-L$(PLAT)/msp430rt/$(L) -l$(L))

$(MSP430DEPS):
	make -C $(dir $@)
