import eval
import sys
import os

# Note: the working directory must contain an eval.py file

def usage():
    print('usage: python run-eval.py <precise-output> <results-directory> <output-file>')
    exit()

if __name__ == "__main__":

    if (len(sys.argv) != 4):
        usage()

    orig = sys.argv[1]
    resdir = sys.argv[2]
    outfile = sys.argv[3]

    files = sorted(os.listdir(resdir))
    fout = open(outfile,'w')
    for fname in files:
        error = eval.score(orig,os.path.join(resdir,fname))
        line = fname+",{:16.16f}".format(error)
        print(line)
        fout.write(line+'\n')

    fout.close()
