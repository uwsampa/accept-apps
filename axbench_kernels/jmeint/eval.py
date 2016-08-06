import os
import sys
import numpy as np

EXT = '.data'

# Arbitrarily large SNR to indicate identical values
SNR_MAX = 200.0

def _read(fn):
    intersects = []

    with open(fn) as f:
        f.readline()  # Skip the count.
        for line in f:
            line = line.strip()
            if line:
                intersects.append(int(line.strip()))

    return intersects

def score(orig, relaxed):
    if not os.path.isfile(relaxed):
        return 1.0
    else:

        goldenData = _read(orig)
        relaxedData = _read(relaxed)

        if len(goldenData) != len(relaxedData):
            # Length mismatch; output is broken.
            return 1.0
        elif (np.array(goldenData)==np.array(relaxedData)).all():
            return SNR_MAX
        else:
            mismatches = 0
            # Get the classification rate
            for idx in range(len(goldenData)):
                if goldenData[idx] != relaxedData[idx]:
                    mismatches += 1
            # Convert to log scale (SNR)
            snr = 20 * np.log10( len(goldenData)/float(mismatches) )
            return snr

if __name__ == '__main__':
    print score('orig'+EXT, 'out'+EXT)

