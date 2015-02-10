from subprocess import check_output

def load():
    output = check_output(["cat", "snr.txt"])
    # print("loaded err = " + output)
    return output 

# The driver computes the snr as part of its own checking process,
# so we just have it write that to a file and then read it back.

def score(orig, relaxed):
    orig_snr = float(orig)
    relaxed_snr = float(relaxed)

    # print("orig: " + orig + "relaxed: " + relaxed)
    # print("  diff: " + str(abs(orig_snr - relaxed_snr)))
    err = abs(orig_snr  - relaxed_snr) / orig_snr
    # print("  err = " + str(err))
    return err
