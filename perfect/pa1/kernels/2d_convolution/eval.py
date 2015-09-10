from subprocess import check_output
import os

EXT = ".mat"

def load():
    output = check_output(["octave", "-q", "assess.m"])
    # print("loaded err = " + output)
    return output

# assess.m computes the average pixel difference between the output
# and the provided reference output, averaged over all trials in the batch
# (default 30 trials, you'll have to modify both the c files and the
# octave script to agree on a different value).


def score(orig, relaxed):
    if (os.path.isfile(relaxed)):
        return float(check_output(["octave", "-q", "assess.m", orig, relaxed]))
    else:
        return 1.0
