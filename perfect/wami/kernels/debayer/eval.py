from subprocess import check_output
import numpy as np
import os
import sys
sys.path.append('../../../utils/')
import perfectlib

EXT = ".bin"

# Evaluate the PSNR between output image and golden image

def score(golden, relaxed):
    return perfectlib.computePSNR(golden, relaxed, "RGBbin")

if __name__ == '__main__':
    print score('orig'+EXT, 'out'+EXT)
