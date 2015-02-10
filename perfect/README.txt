This directory contains most of the kernels from the PERFECT benchmark suite,
annotated for compilation with accept. The composite (app) tests are not,
nor is the sorting kernel, as it didn't seem like a good testbed to deal with
approximate sorting.

Most of the input / golden output files necessary to run the benchmarks have
been omitted, as they are quite large. They can be downloaded along with the
original suite at http://hpc.pnl.gov/PERFECT/. This work is based on version
1.0.

Conveniently, the required kernels produce their own random inputs, so they
should work fine.

The small inputs have been included for the pa1 benchmarks, alongside the
source files. The medium and large inputs should go in the same place.

The other benchmarks (sar, stap, and wami) expect their inputs to be in a
directory called inout/ in each of their corresponding source directories.
You can just copy the inout/ directory from the suite (i.e. suite/sar/inout),
though you may not want to duplicate unused inputs if you're low on disk space.

Some of the benchmarks expect octave to be installed to run the quality metric.
