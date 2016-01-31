import os
import sys
sys.path.append('../../../utils/')
import perfectlib

EXT = ".mat"
INPUT_FN = "input_small"+EXT

def score(orig, relaxed):
    if (os.path.isfile(relaxed)):
        return perfectlib.computeSNR(orig, relaxed, "fft")
    else:
        return 1.0

if __name__ == '__main__':
    print score(INPUT_FN, 'out'+EXT)
