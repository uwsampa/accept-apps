import os
import sys
sys.path.append('../../../utils/')
import perfectlib

EXT = ".bin"

def score(orig, relaxed):
    if (os.path.isfile(relaxed)):
        return perfectlib.computeSNR(orig, relaxed, "wami")
    else:
        return 1.0

if __name__ == '__main__':
    print score('orig'+EXT, 'out'+EXT)
