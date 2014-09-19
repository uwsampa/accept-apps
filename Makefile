SUBDIRS = blackscholes jpeg streamcluster sobel canneal fluidanimate x264 zynq-sobel zynq-jmeint zynq-inversek2j zynq-fft ccv zynq-blackscholes
CLEANDIRS = $(SUBDIRS:%=clean-%)

.PHONY: clean $(CLEANDIRS)
clean: $(CLEANDIRS)
$(CLEANDIRS):
	$(MAKE) -C $(@:clean-%=%) clean


# Experiments for the paper.

.PHONY: exp exp_setup

ACCEPT_DIR := ../accept
APPS := streamcluster sobel canneal fluidanimate x264
EXP_PY_DEPS := munkres pillow
PARSEC_INPUTS := \
	canneal/40000.nets \
	canneal/2500000.nets \
	fluidanimate/in_100K.fluid \
	fluidanimate/in_300K.fluid \
	x264/eledream_640x360_32.y4m \
	x264/eledream_640x360_128.y4m

# Reduce reps with MINI=1.
ifneq ($(MINI),)
ACCEPT_ARGS := -r1 -R1
else
ACCEPT_ARGS := -r2 -R5
endif

# Get a non-timing run with NOTIME=1. Disables forcing.
ifeq ($(NOTIME),)
ACCEPT_ARGS += -f
EXP_ARGS += -t
endif

# Run on cluster with CLUSTER=1.
ifneq ($(CLUSTER),)
ACCEPT_ARGS += -c
endif

# Zynq platform experiments.
ifeq ($(EXPLAT),zynq)
APPS := zynq-inversek2j zynq-sobel zynq-blackscholes
endif

exp:
	$(ACCEPT_DIR)/bin/accept $(ACCEPT_ARGS) -vv exp -j $(EXP_ARGS) $(APPS)

exp_setup:
	$(ACCEPT_DIR)/venv/bin/pip install $(EXP_PY_DEPS)
ifeq ($(CLUSTER),1)
	$(ACCEPT_DIR)/venv/bin/pip install git+git://github.com/sampsyo/cluster-workers.git
endif
