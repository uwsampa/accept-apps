#!/usr/bin/env python
import argparse
import os
import glob
import numpy as np
import matplotlib.pyplot as plt
from matplotlib import lines
import seaborn as sns
from scipy import interpolate

MAX_DB = 120.0

# Seaborn Settings
sns.set_context("poster")

# String map for renaming PERFECT applications
stringMap = {
    "pa1": "PA1",
    "stap": "STAP",
    "sar": "SAR",
    "wami": "WAMI",
    "required": "Required",
    "2d-convolution": "2d-conv",
    "dwt53": "dwt53",
    "histogram-equal": "hist-equal",
    "innter-product": "innter-product",
    "outer-product": "outer-product",
    "system-solve": "system-solve",
    "pfa-interp1": "pfa-interp1",
    "pfa-interp2": "pfa-interp2",
    "debayer": "debayer",
    "change-detection": "gmm",
    "lucas-kanade": "lucas-kanade",
    "fft-1d": "fft-1d",
    "fft-2d": "fft-2d"
}

def simpleVisualization():
    db_range = [10, 20, 40, 60, 80, 100, 120]
    bit_savings = [93,83,75,70,67,63,55]
    memory_savings = [0.875772361,0.8392786702,0.7892933058,0.7601111821,0.731401315,0.694701043,0.616858759]
    exe_savings = [0.9177500644,0.8120697499,0.7024116917,0.6426573056,0.6032877772,0.5664314329,0.4863831787]
    fp_savings = [0.9178708019,0.8075741525,0.6245486779,0.5532578172,0.5128679032,0.4753819891,0.389494369]
    int_savings = [0.92031818130,.8936952261,0.8472632767,0.8233294395,0.8116106895,0.8037981895,0.8015685351]

    width = 0.20
    ind = np.array(range(len(db_range)))

    plt.plot(db_range, memory_savings, label="Average Memory Bit-Savings", linestyle='-')
    plt.plot(db_range, fp_savings, label="Average FP Ops Bit-Savings", linestyle='--')
    plt.plot(db_range, int_savings, label="Average Int Ops Bit-Savings", linestyle=':')

    # add some text for labels, title and axes ticks
    plt.xlabel('Quality (SNR - higher is better)')
    plt.ylabel('Compute Bit Savings (higher is better)')
    plt.legend()
    plt.show()

def findBest(csv, idx, threshold):
    best = 0
    for i, elem in enumerate(csv):
        if float(elem[idx]) > threshold:
            best = i
    return csv[best]

def getFileList(path):
    flist = glob.glob(path+"*.log")

    results = []
    for f in flist:
        results.append(process(f))

    plotResults(results)

def plotResults(results, size=5, smoothing=False):

    palette = sns.color_palette()
    lstyle = ['-','--',':']

    f, axarr = plt.subplots(size, 2, sharex=True)

    benchMap = {
        "pa1": 0,
        "stap": 1,
        "sar": 2,
        "wami": 3,
        "required": 4
    }

    benchCount = {
        "pa1": 0,
        "stap": 0,
        "sar": 0,
        "wami": 0,
        "required": 0
    }

    for idx, bench in enumerate(results):
        cat = bench["name"].split("_")[0]
        label = bench["name"].split("_")[1]

        axis0 = axarr[benchMap[cat]][0]
        axis1 = axarr[benchMap[cat]][1]
        csv = bench["csv"]

        # Derive useful CSV indices
        errorIdx = csv[0].index("error")
        exeIdx = csv[0].index("exe")
        memIdx = csv[0].index("mem")

        # Get data points
        x = [float(p[errorIdx]) for p in csv[1:]]
        y0 = [float(p[exeIdx]) for p in csv[1:]]
        y1 = [float(p[memIdx]) for p in csv[1:]]

        # Optional smoothing
        if smoothing:
            x = np.array(x[::-1])
            y0 = np.array(y0[::-1])
            y1 = np.array(y1[::-1])
            tck0 = interpolate.splrep(x, y0, s=0)
            tck1 = interpolate.splrep(x, y1, s=0)
            xnew = np.arange(x.min(), x.max(), (x.max()-x.min())/500)
            y0new = interpolate.splev(xnew, tck0, der=0)
            y1new = interpolate.splev(xnew, tck1, der=0)
            x = xnew
            y0 = y0new
            y1 = y1new

        axis0.plot(x, y0, color=palette[benchCount[cat]], label=label, linestyle=lstyle[benchCount[cat]])
        axis0.set_xlim([0,120])
        axis0.set_ylim([-.02,1.02])
        axis0.tick_params(axis='both', which='major', labelsize=15)
        axis0.set_title(stringMap[cat], fontsize=12)

        axis1.plot(x, y1, color=palette[benchCount[cat]], label=label, linestyle=lstyle[benchCount[cat]])
        axis1.set_xlim([0,120])
        axis1.set_ylim([-.02,1.02])
        axis1.tick_params(axis='both', which='major', labelsize=15)
        axis1.set_title(stringMap[cat], fontsize=12)

        # Increment benchcount
        benchCount[cat]+=1

    # Individualized legends
    axarr[0][0].legend(loc="upper right", ncol=2, fontsize=11)
    axarr[1][0].legend(loc="lower center", ncol=3, fontsize=11)
    axarr[2][0].legend(loc="lower left", ncol=3, fontsize=11)
    # axarr[3][0].legend(loc="upper right", ncol=1, fontsize=11)
    axarr[4][0].legend(loc="lower center", ncol=2, fontsize=11)

    axarr[0][1].legend(loc="lower center", ncol=3, fontsize=11)
    axarr[1][1].legend(loc="lower center", ncol=3, fontsize=11)
    axarr[2][1].legend(loc="lower center", ncol=3, fontsize=11)
    axarr[3][1].legend(loc="lower left", ncol=1, fontsize=11)
    axarr[4][1].legend(loc="lower center", ncol=2, fontsize=11)

    # Labels
    axarr[size-1][0].set_xlabel('Quality (SNR - higher is better)')
    axarr[size-1][1].set_xlabel('Quality (SNR - higher is better)')
    axarr[size//2][0].set_ylabel('Compute Bit Savings (higher is better)')
    axarr[size//2][1].set_ylabel('Memory Bit Savings (higher is better)')

    plt.savefig("results.pdf", bbox_inches='tight')


def process(fn, pareto=True, plotMe=False, arithRatio=None, memoryRatio=None):

    # Benchmark name
    bench = fn.split("/")[-1].split('.')[0]
    print "Processing benchmark {}".format(bench)

    # First read in the file in CSV format
    csv = []
    with open(fn) as f:
        lines = f.readlines()
        for l in lines:
            csv.append(l.strip().split('\t'))

    # Derive useful CSV indices
    errorIdx = csv[0].index("error")
    memIdx = csv[0].index("mem")
    exeIdx = csv[0].index("exe")
    fpIdx = csv[0].index("exe_fp")
    intIdx = csv[0].index("exe_int")

    if pareto:
        # Select the pareto-optimal points
        paretoCsv = [csv[0]]
        for i, elem in enumerate(csv[1:]):
            paretoOptimal = True
            for j in range(i+1,len(csv)):
                if float(csv[j][errorIdx]) > float(elem[errorIdx]):
                    paretoOptimal = False
            if paretoOptimal:
                paretoCsv.append(elem)
        csv = paretoCsv

    # Record error at 20dB, 40dB, 60dB
    thresholds = []
    for dB in [10]+range(20,121,20):
        thresholds.append([dB, findBest(csv[1:], errorIdx, dB)])

    # Report bit reduction savings at each threshold
    # csvLine = []
    for t in thresholds:
        print "Best error above {}dB: {}\t{}\t{}".format(t[0], t[1][errorIdx], t[1][memIdx], t[1][exeIdx])
    #     csvLine.append(t[1][intIdx])
    # print "\t".join(csvLine)

    if plotMe:
        y1 = [float(x[memIdx]) for x in csv[1:]]
        y2 = [float(x[exeIdx]) for x in csv[1:]]
        y = [(s[0]*memoryRatio + s[1]*arithRatio)/(memoryRatio+arithRatio) for s in zip(y1, y2)]
        x = [x[errorIdx] for x in csv[1:]]

        plt.plot(x, y, label=fn)
        plt.xlim([0,120])
        plt.ylim([-.02,1.02])

        plt.xlabel('SNR (dB - higher is better)')
        plt.ylabel('Bit-Savings (higher is better)')

        plt.savefig("plot.pdf", bbox_inches='tight')

    return {
        "name": bench,
        "csv": csv,
        "thresholds": thresholds
    }

def cli():
    parser = argparse.ArgumentParser(
        description='Process and visualize precision autotuner error log file.'
    )
    parser.add_argument(
        '-p', dest='logPath', action='store', type=str, required=True,
        default=None, help='error log directory'
    )
    parser.add_argument(
        '-arithRatio', dest='arithRatio', action='store', type=float, required=False,
        default=0.5, help='ratio of dynamically executed approximate arithmetic instructions'
    )
    parser.add_argument(
        '-memoryRatio', dest='memoryRatio', action='store', type=float, required=False,
        default=0.5, help='ratio of dynamically executed approximate memory instructions'
    )
    args = parser.parse_args()

    if os.path.isdir(args.logPath):
        getFileList(args.logPath)
    elif(os.path.exists(args.logPath)):
        process(args.logPath, pareto=True, plotMe=True, arithRatio=args.arithRatio, memoryRatio=args.memoryRatio)
    else:
        print "Error: {} not found!"
        exit()

if __name__ == '__main__':
    cli()


