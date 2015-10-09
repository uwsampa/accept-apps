import sys
sys.path.append('../../../utils/')
import perfectlib

EXT = ".mat"

# Evaluate the SNR between the golden and approximate Hessian matrices
# The precise execution produces a snr of about 122, and the PERFECT
# documentation suggests an acceptable SNR is in the range of about 100 dB.

def score(golden, relaxed):
    return perfectlib.computeSNR(golden, relaxed, "mat")

if __name__ == '__main__':
    print score('orig'+EXT, 'out'+EXT)
