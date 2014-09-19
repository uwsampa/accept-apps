BASEDIR    := $(ACCEPT_DIR)/../accept-apps

LINKER     := msp430-gcc
LLCARGS    += -march=msp430 -msp430-hwmult-mode=no
LDFLAGS    += -mmcu=msp430fr5969
RUNSHIM    := $(BASEDIR)/plat/msp430/run.sh
MSP430LIBS += perfctr
MSP430DEPS := $(foreach L,$(MSP430LIBS),$(BASEDIR)/msp430rt/$(L)/lib$(L).a)
EXTRADEPS  += $(MSP430DEPS)
override \
CFLAGS     += -target msp430-elf -Wall -fno-stack-protector -D__MSP430FR5969__ \
		$(addprefix -I, \
			$(shell msp430-cpp -Wp,-v </dev/null 2>&1 | \
			grep /include | sed -e 's/^ *//')) \
		$(addprefix -I$(BASEDIR)/plat/msp430rt/,$(MSP430LIBS))
LIBS       += $(foreach L,$(MSP430LIBS),-L$(BASEDIR)/plat/msp430rt/$(L) -l$(L))

$(MSP430DEPS):
	make -C $(dir $@)
