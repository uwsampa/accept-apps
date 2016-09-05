import os
import sys
sys.path.append('../../../utils/')
import perfectlib

EXT = ".mat"

def score(orig, relaxed):
    if (os.path.isfile(relaxed)):
        return perfectlib.computeSNR(orig, relaxed, "mat")
    else:
        return 1.0

def qorTest(orig, relaxed, qor_mode, delta, confidence):
    if (os.path.isfile(relaxed)):
        if qor_mode=="stat":
            return perfectlib.clopperPearson(orig, relaxed, "mat", delta, confidence)
        elif qor_mode=="worst":
            return perfectlib.worstError(orig, relaxed, "mat", delta)
        elif qor_mode=="avg":
            return perfectlib.averageError(orig, relaxed, "mat", delta)
        else:
            return 0
    else:
        return 0

if __name__ == '__main__':
    print score('orig'+EXT, 'out'+EXT)
