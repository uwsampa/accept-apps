import os
import sys
import numpy as np

EXT = '.dat'

# Arbitrarily large SNR to indicate identical values
SNR_MAX = 200.0

def _read(fn):
    rgb = []

    with open(fn) as f:
        dim  = f.readline()
        w = 3*int(dim.split(",")[0])
        h = int(dim.split(",")[1])
        f.readline() # Skip first line
        for idx, line in enumerate(f):
            if idx>=h-2:
                break
            line = line.strip()
            if line:
                tokens = line.split(",")
                rgb.append([float(x) for x in tokens[3:w-3]])

    return np.array(rgb)

def score(orig, relaxed):
    if not os.path.isfile(relaxed):
        return 1.0
    else:

        goldenData = _read(orig)
        relaxedData = _read(relaxed)

        if len(goldenData) != len(relaxedData):
            # Length mismatch; output is broken.
            return 1.0
        elif (goldenData==relaxedData).all():
            return SNR_MAX
        else:
            # Here we compute the SNR
            num = ((goldenData) ** 2).sum(axis=None)
            den = ((goldenData - relaxedData) ** 2).sum(axis=None)
            snr = 10 * np.log10( num/den )
            return snr

if __name__ == '__main__':
    print score('orig'+EXT, 'out'+EXT)

