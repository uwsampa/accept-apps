from subprocess import check_output

def load():
    output = check_output(["octave", "-q", "assess.m"])
    # print("loaded snr = " + output)
    return output 

# assess.m is an octave script that computes a signal to noise ratio in dB
# by comparing to the "ideal" output produced by octave's fft function.

# The precise execution produces a snr of about 125, and the PERFECT
# documentation suggests an acceptable snr is in the range of about 100 dB.

def score(orig, relaxed):
    orig_snr = float(orig)
    relaxed_snr = float(relaxed)

    # print("orig: " + orig + "relaxed: " + relaxed)
    # print("  diff: " + str(abs(orig_snr - relaxed_snr)))
    err = abs(orig_snr  - relaxed_snr) / orig_snr
    # print("  err = " + str(err))
    return err
