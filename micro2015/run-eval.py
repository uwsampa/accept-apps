import eval
import sys

if __name__ == "__main__":
    error = eval.score(sys.argv[1],sys.argv[2])
    print("error: {:16.16f}".format( error))
