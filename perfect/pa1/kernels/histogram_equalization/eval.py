from subprocess import check_output
import os

EXT = ".mat"

def load():
    output = check_output(["octave", "-q", "assess.m"])
    return output

# assess.m is an octave script that computes a signal to noise ratio in dB
# by comparing a reference hist_ew kernel output to an approximate output.


def score(orig, relaxed):
    if (os.path.isfile(relaxed)):
        return float(check_output(["octave", "-q", "assess.m", orig, relaxed]))
    else:
        return 1.0