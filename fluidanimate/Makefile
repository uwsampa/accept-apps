TARGET := fluidanimate

# RUNARGS := 8 5 in_100K.fluid out.fluid  # simmedium
RUNARGS := 8 5 in_300K.fluid out.fluid  # simlarge
TESTARGS := 8 10 in_500K.fluid out.fluid  # native

SOURCES := pthreads.cpp cellpool.cpp parsec_barrier.cpp liberror.cpp enerj.cpp
HEADERS := cellpool.hpp fluid.hpp parsec_barrier.hpp enerj.hpp
LDFLAGS := -lpthread
CXXFLAGS += -pthread -D_GNU_SOURCE -D__XOPEN_SOURCE=600 -Wno-invalid-offsetof 
CLEANMETOO := fluidcmp out.fluid

ACCEPT_DIR := ../../accept
include $(ACCEPT_DIR)/accept.mk

# Output evaluation tool. Not approximated.
fluidcmp: fluidcmp.cpp
	$(CXX) $(CXXFLAGS) $< -o $@

# Compile the evaluation tool before the workflow begins.
setup: fluidcmp
