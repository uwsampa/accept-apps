from subprocess import check_output
import os

EXT = ".txt"

def load():
    output = check_output(["cat", "err.txt"])
    return output

# The driver computes the average pixel difference for us,
# just read it in

def score(orig, relaxed):
    if (os.path.isfile(relaxed)):
        return check_output(["cat", relaxed])
    else:
        return 1.0

