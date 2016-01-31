from subprocess import check_output
import os
import math
import argparse
import struct
import numpy as np
from numpy import linalg as la
from matplotlib import cm, pyplot as plt

# Arbitrarily large SNR to indicate identical values
SNR_MAX = 200.0
# Arbitrarily small SNR to indicate different values
SNR_MIN = 1.0

def load_mat(filename):
    mat = []
    with open(filename) as f:
        for line in f:
            if line.strip()!='':
                line = line.strip().split(" ")
                if line[0]!='%':
                    line = [float(x) for x in line]
    return np.array(mat)

def load_fft(filename):
    mat = []
    with open(filename) as f:
        for line in f:
            if line.strip()!='' and line[0]!='#':
                line = line.strip()[1:-1].split(") (")
                line = [x.split(', ') for x in line]
                line = [float(x[0])+1j*float(x[1]) for x in line]
                mat.append(line)
    return np.array(mat)

def load_stap_bin(filename):
    data = []
    f = open(filename, "rb")
    try:
        word = [0, 0]
        word[0] = f.read(4)
        word[1] = f.read(4)
        while word[0] != "" or word[1] != "":
            (re,) = struct.unpack('f', word[0])
            (im,) = struct.unpack('f', word[1])
            elem = re + 1j * im
            data.append(elem)
            word[0] = f.read(4)
            word[1] = f.read(4)
    finally:
        f.close()

    return np.array(data)

def load_bin(filename, luma=False, metadata=False):
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

    if metadata:
        print("Image width = {}".format(params[0]))
        print("Image height = {}".format(params[1]))
        print("Channels = {}".format(params[2]))
        print("Depth = {}".format(params[3]))
        print("Pixel count = {}".format(len(pixels)))

    channels = params[2]

    # Normalize
    minVal = min(pixels)
    maxVal = max(pixels)
    # pixels = [(x-minVal)/(maxVal-minVal) for x in pixels]

    mat = []
    for y in range(0, params[1]):
        row = pixels[y*channels*params[0]:(y+1)*channels*params[0]]
        if channels==1:
            mat.append(row)
        elif channels == 3:
            rgb_row = []
            for x in range(0, params[0]):
                if luma:
                    # using RGB to lumincance conversion for ITU-R BT.709 / sRGB
                    r = row[x*channels+0]
                    g = row[x*channels+1]
                    b = row[x*channels+2]
                    luminance = 0.2126*r + 0.7152*g + 0.0722*b
                    rgb_row.append(luminance)
                else:
                    rgb_pixel = (row[x*channels+0],row[x*channels+1],row[x*channels+2])
                    rgb_row.append(rgb_pixel)
            mat.append(rgb_row)

    return np.array(mat)

def computeSNR(golden, relaxed, mode):
    if (os.path.isfile(relaxed)):
        if mode=="stap":
            goldenData = load_stap_bin(golden)
            relaxedData = load_stap_bin(relaxed)
            if (goldenData==relaxedData).all():
                return SNR_MAX
            else:
                # SNR computation template for STAP kernels
                # taken from stap/kernels/ser/lib/stap_utils.c
                # from calculate_snr() definition
                den = 0
                num = 0
                for i in range(len(goldenData)):
                    den += (goldenData[i].real - relaxedData[i].real) * \
                           (goldenData[i].real - relaxedData[i].real)
                    den += (goldenData[i].imag - relaxedData[i].imag) * \
                           (goldenData[i].imag - relaxedData[i].imag)
                    num += goldenData[i].real * goldenData[i].real + \
                           goldenData[i].imag * goldenData[i].imag
                snr = 10 * np.log10( num/den );
                return snr
        elif mode=="fft":
            # Here we recompute a golden FFT from input data
            relaxedData = load_fft(relaxed)
            inputData = load_fft(golden)
            if inputData.shape[0] == 1:
                goldenData = np.fft.fft(inputData)
            else:
                goldenData = np.fft.fft2(inputData)
            relaxedData = load_fft(relaxed)
            if (goldenData==relaxedData).all():
                return SNR_MAX
            else:
                # SNR computation template for STAP kernels
                # taken from stap/kernels/ser/lib/stap_utils.c
                # from calculate_snr() definition
                # in order to compute SNR of imaginary matrices.
                # Works identically for 1-D function to octave
                # implementation in required/ser/fft-1d/assess.m
                # but produces slightly different result for 2-D
                # function as detailed in required/ser/fft-2d/assess.m
                den = 0
                num = 0
                for i in range(len(goldenData)):
                    for j in range(len(goldenData[i])):
                        den += (goldenData[i][j].real - relaxedData[i][j].real) * \
                               (goldenData[i][j].real - relaxedData[i][j].real)
                        den += (goldenData[i][j].imag - relaxedData[i][j].imag) * \
                               (goldenData[i][j].imag - relaxedData[i][j].imag)
                        num += goldenData[i][j].real * goldenData[i][j].real + \
                               goldenData[i][j].imag * goldenData[i][j].imag
                snr = 10 * np.log10( num/den );
                return snr
        elif mode=="mat":
            goldenData = load_mat(golden)
            relaxedData = load_mat(relaxed)
            if (goldenData==relaxedData).all():
                return SNR_MAX
            else:
                # Here we compute the SNR based on the PERFECT doc
                num = ((goldenData) ** 2).sum(axis=None)
                den = ((goldenData - relaxedData) ** 2).sum(axis=None)
                snr = 10 * np.log10( num/den );
                return snr
        else:
            return SNR_MIN
    else:
        return SNR_MIN

def computePSNR(golden, relaxed, mode):
    if (os.path.isfile(relaxed)):
        if mode=="RGBbin":
            goldenData = load_bin(golden, luma=True)
            relaxedData = load_bin(relaxed, luma=True)
            if (goldenData==relaxedData).all():
                return SNR_MAX
            else:
                # For details on how to compute PSNR in multimedia applications
                # https://en.wikipedia.org/wiki/Peak_signal-to-noise_ratio
                mseVal = ((goldenData - relaxedData) ** 2).mean(axis=None)
                maxVal = np.amax(goldenData)
                psnr = 20 * np.log10(maxVal) - 10 * np.log10(mseVal);
                return psnr
        else:
            return SNR_MIN
    else:
        return SNR_MIN

def display(fn):
    if fn.endswith('.mat'):
        img_array = load_mat(fn)
        plt.imshow(img_array, interpolation='nearest', cmap = cm.Greys_r)
        plt.show()
    elif fn.endswith('.bin'):
        img_array = load_bin(fn, luma=True)
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

    display(args.path)

if __name__ == '__main__':
    cli()
