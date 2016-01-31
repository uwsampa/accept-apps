import os
import sys
sys.path.append('../../../utils/')
import perfectlib

EXT = ".bin"
GOLDEN_FN = 'small_kernel1_output'+EXT

def score(orig, relaxed):
    if (os.path.isfile(relaxed)):
        return perfectlib.computeSNR(orig, relaxed, "stap")
    else:
        return 1.0

if __name__ == '__main__':
    print score(GOLDEN_FN, 'out'+EXT)
