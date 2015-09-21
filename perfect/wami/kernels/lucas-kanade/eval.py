from subprocess import check_output
import os

EXT = ".mat"

GOLDEN_FN = "golden"+EXT

def load():
    output = check_output(["octave", "-q", "assess.m"])
    return output

# assess.m is an octave script that computes a signal to noise ratio in dB
# by comparing to the "ideal" output produced by octave's fft function.

# The precise execution produces a snr of about 125, and the PERFECT
# documentation suggests an acceptable snr is in the range of about 100 dB.

def score(orig, relaxed):
    if (os.path.isfile(relaxed)):
        orig_snr = float(check_output(["octave", "-q", "assess.m", GOLDEN_FN, orig]))
        relaxed_snr = float(check_output(["octave", "-q", "assess.m", GOLDEN_FN, relaxed]))
        err = abs(orig_snr  - relaxed_snr) / orig_snr
        return err
    else:
        return 1.0