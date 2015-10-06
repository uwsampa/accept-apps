from subprocess import check_output
import os
import math
import argparse
import struct
import numpy as np
from matplotlib import cm, pyplot as plt

def load_mat(filename):
    mat = []
    with open(filename) as f:
        for line in f:
            line = line.strip().split(" ")
            if line[0]!='%':
                line = [int(x) for x in line]
                mat.append(line)
    return np.array(mat)


def load_bin(filename):
    pixels = []
    params = []
    f = open(filename, "rb")
    try:
        word = f.read(2)
        while word != "":
            if len(params)<4:
                params.append(struct.unpack('h', word)[0])
                word = f.read(2)
            else:
                pix = struct.unpack('h', word)[0]
                pixels.append(pix)
                word = f.read(params[3])
    finally:
        f.close()

    print("Image width = {}".format(params[0]))
    print("Image height = {}".format(params[1]))
    print("Channels = {}".format(params[2]))
    print("Depth = {}".format(params[3]))
    print("Pixel count = {}".format(len(pixels)))

    channels = params[2]

    # Normalize
    maxVal = max(pixels)
    pixels = [float(x)/maxVal for x in pixels]

    mat = []
    for y in range(0, params[1]):
        row = pixels[y*channels*params[0]:(y+1)*channels*params[0]]
        if channels > 1:
            rgb_row = []
            for x in range(0, params[0]):
                rgb_pixel = []
                for c in range(channels):
                    rgb_pixel.append(row[x*channels+c])
                rgb_row.append(rgb_pixel)
            mat.append(rgb_row)
        else:
            mat.append(row)

    return np.array(mat)



def process(fn):
    if fn.endswith('.mat'):
        img_array = load_mat(fn)
        plt.imshow(img_array, interpolation='nearest', cmap = cm.Greys_r)
        plt.show()
    elif fn.endswith('.bin'):
        img_array = load_bin(fn)
        print img_array[0]
        channels = 1 if len(img_array.shape)==2 else img_array.shape[2]
        if channels==1:
            plt.imshow(img_array, interpolation='nearest', cmap = cm.Greys_r)
        else:
            plt.imshow(img_array)
        plt.show()





def cli():
    parser = argparse.ArgumentParser(
        description='Displays image'
    )
    parser.add_argument(
        '-f', dest='path', action='store', type=str, required=True,
        default=None, help='path to file'
    )
    args = parser.parse_args()

    process(args.path)

if __name__ == '__main__':
    cli()
