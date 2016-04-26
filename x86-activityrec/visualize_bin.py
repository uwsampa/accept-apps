from subprocess import check_output
import os
import math
import argparse
import struct
import numpy as np
import seaborn as sns
from numpy import linalg as la
from matplotlib import cm, pyplot as plt

WINDOW = 4

def load_bin(filename):
    x_buf = []
    y_buf = []
    z_buf = []
    data_idx = 0
    with open(filename, "rb") as f:
        byte = f.read(1)
        while byte != "":
            b = ord(byte)
            if data_idx%4==0:
                x_buf.append(b)
            elif data_idx%4==1:
                y_buf.append(b)
            elif data_idx%4==2:
                z_buf.append(b)
            byte = f.read(1)
            data_idx += 1

    # Style
    sns.set_style("white")

    # X-time bounds:
    xmin = 50
    xmax = 300

    x = range(xmin, xmax)
    y = z_buf[xmin:xmax]
    plt.plot(x, y)
    # y0 = x_buf[xmin:xmax]
    # y1 = y_buf[xmin:xmax]
    # y2 = z_buf[xmin:xmax]

    # f, axarr = plt.subplots(3, sharex=True)
    # axarr[0].plot(x, y0)
    # axarr[1].plot(x, y1)
    # axarr[2].plot(x, y2)
    plt.show()


def display(path):
    load_bin(path)


def cli():
    parser = argparse.ArgumentParser(
        description='Visualizes the activity recognition data'
    )
    parser.add_argument(
        '-f', dest='path', action='store', type=str, required=True,
        default=None, help='path to file'
    )
    args = parser.parse_args()

    display(args.path)

if __name__ == '__main__':
    cli()
