from subprocess import check_output

def load():
    output = check_output(["cat", "err.txt"])
    # print("loaded err = " + output)
    return output 

# The driver computes the average pixel difference for us,
# just read it in


def score(orig, relaxed):
    return float(relaxed)
