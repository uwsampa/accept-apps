import os
import sys
import numpy as np

EXT = '.data'

# Arbitrarily large SNR to indicate identical values
SNR_MAX = 200.0

def _read(fn):
    thetas = []

    with open(fn) as f:
        f.readline()  # Skip the count.
        for line in f:
            line = line.strip()
            if line:
                tokens = line.split("\t")
                thetas.append([float(x) for x in tokens])

    return np.array(thetas)

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

