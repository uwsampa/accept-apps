from subprocess import check_output
import os
import scipy.io
import math
import numpy as np
from matplotlib import pyplot as plt
from matplotlib import cm as cm
import pywt

EXT = ".mat"

def load():
    output = check_output(["octave", "-q", "assess.m"])
    # print("loaded err = " + output)
    return output

# assess.m is an octave script that computes a signal to noise ratio in dB
# by comparing to the original input to the output of the dwt kernel.

# The precise execution produces infinite snr since the compression is lossless.

def score(orig, relaxed):
    if (os.path.isfile(relaxed)):
        return float(check_output(["octave", "-q", "assess.m", orig, relaxed]))
    else:
        return 1.0