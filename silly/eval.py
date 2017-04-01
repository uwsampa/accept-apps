def load(fn='output.txt'):
    with open(fn) as f:
        return int(f.read().strip())

def score(orig, relaxed):
    return abs(orig - relaxed)
