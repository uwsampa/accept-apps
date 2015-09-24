from subprocess import check_output
import os

EXT = ".txt"

def load():
    output = check_output(["cat", "snr.txt"])
    return output

# The driver computes the snr as part of its own checking process,
# so we just have it write that to a file and then read it back.

def score(orig, relaxed):
    if (os.path.isfile(relaxed)):
        # orig_snr = float(check_output(["cat", orig]))
        relaxed_snr = float(check_output(["cat", relaxed]))
        # err = abs(orig_snr  - relaxed_snr) / orig_snr
        return relaxed_snr
    else:
        return 1.0
