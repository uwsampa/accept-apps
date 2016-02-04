from __future__ import division
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

# WAMI params (for INPUT_SIZE_SMALL)
# Taken from wami/kernels/ser/change-detection/lib/wami_params.h
WAMI_GMM_IMG_NUM_ROWS = 512
WAMI_GMM_IMG_NUM_COLS = 512
WAMI_GMM_NUM_FRAMES = 5

# WAMI morpho erode, as defined in
# wami/kernels/ser/change-detection/wami_morpho.c
# Used for quality evaluation.
def wami_morpho_erode(frame):
    eroded = [[0]*WAMI_GMM_IMG_NUM_COLS for r in range(WAMI_GMM_IMG_NUM_ROWS)]
    for row in range(WAMI_GMM_IMG_NUM_ROWS):
        for col in range(WAMI_GMM_IMG_NUM_COLS):
            row_m_1 = row-1 if (row>0) else 0
            col_m_1 = col-1 if (col>0) else 0
            row_p_1 = row+1 if (row<WAMI_GMM_IMG_NUM_ROWS-1) else WAMI_GMM_IMG_NUM_ROWS-1
            col_p_1 = col+1 if (col<WAMI_GMM_IMG_NUM_COLS-1) else WAMI_GMM_IMG_NUM_COLS-1
            if (frame[row][col] == 0 or
                frame[row_m_1][col_m_1] == 0 or
                frame[row_m_1][col] == 0 or
                frame[row_m_1][col_p_1] == 0 or
                frame[row][col_m_1] == 0 or
                frame[row][col] == 0 or
                frame[row][col_p_1] == 0 or
                frame[row_p_1][col_m_1] == 0 or
                frame[row_p_1][col] == 0 or
                frame[row_p_1][col_p_1] == 0):
                eroded[row][col] = 0
            else:
                eroded[row][col] = 1
    return eroded

def load_mat(filename):
    mat = []
    with open(filename) as f:
        for line in f:
            if line.strip()!='':
                line = line.strip().split(" ")
                if line[0]!='%':
                    line = [float(x) for x in line]
                    mat.append(line)
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

def load_fp_bin(filename):
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

def load_wami_img(filename):
    data = []
    f = open(filename, "rb")
    try:
        char = f.read(1)
        while char != "":
            data.append(ord(struct.unpack('c', char)[0]))
            char = f.read(1)
    finally:
        f.close()

    imgs = []
    numPixels = WAMI_GMM_IMG_NUM_ROWS*WAMI_GMM_IMG_NUM_COLS
    for frame in range(WAMI_GMM_NUM_FRAMES):
        frameBuf = []
        for row in range(WAMI_GMM_IMG_NUM_ROWS):
            frameBuf.append(data[frame*numPixels+row*WAMI_GMM_IMG_NUM_COLS:frame*numPixels+(row+1)*WAMI_GMM_IMG_NUM_COLS])
        imgs.append(frameBuf)
    return np.array(imgs)

def load_img_bin(filename, normalize=False, luma=False, metadata=False):
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

    width = params[0]
    height = params[1]
    channels = params[2]

    if normalize:
        # Normalize
        maxVal = max(pixels)
        pixels = [float(x)/maxVal*255 for x in pixels]

    if luma:
        rgbArray = np.zeros((height,width), 'uint8')
    else:
        rgbArray = np.zeros((height,width,channels), 'uint8')
    for y in range(height):
        for x in range(width):
            if luma:
                # using RGB to lumincance conversion for ITU-R BT.709 / sRGB
                r = pixels[y*width*channels+x*channels+0]
                g = pixels[y*width*channels+x*channels+1]
                b = pixels[y*width*channels+x*channels+2]
                luminance = 0.2126*r + 0.7152*g + 0.0722*b
                rgbArray[y][x] = luminance
            else:
                for c in range(channels):
                    rgbArray[y][x][c] = pixels[y*width*channels+x*channels+c]

    return rgbArray

def computeSNR(golden, relaxed, mode):
    if (os.path.isfile(relaxed)):
        if mode=="stap" or mode=="sar":
            goldenData = load_fp_bin(golden)
            relaxedData = load_fp_bin(relaxed)
            if (goldenData==relaxedData).all():
                return SNR_MAX
            else:
                # SNR computation template for STAP kernels
                # taken from stap/kernels/ser/lib/stap_utils.c
                # from calculate_snr() definition
                den = 0
                num = 0
                for i in range(len(goldenData)):
                    den += (goldenData[i].real - relaxedData[i].real) ** 2
                    den += (goldenData[i].imag - relaxedData[i].imag) ** 2
                    num += goldenData[i].real * goldenData[i].real + \
                           goldenData[i].imag * goldenData[i].imag
                snr = 10 * np.log10( num/den )
                return snr
        elif mode=="wami":
            # change-detection correcness check
            # as implemented in
            # wami/kernels/ser/change-detection/src/wami_kernel3_driver.c
            numMisclassified = 0
            numForeground = 0
            goldenData = load_wami_img(golden)
            relaxedData = load_wami_img(relaxed)
            for goldenFrame, relaxedFrame in zip(goldenData, relaxedData):
                goldedEroded = wami_morpho_erode(goldenFrame)
                relaxedEroded = wami_morpho_erode(relaxedFrame)
                for row in range(WAMI_GMM_IMG_NUM_ROWS):
                    for col in range(WAMI_GMM_IMG_NUM_COLS):
                        if goldedEroded[row][col] != relaxedEroded[row][col]:
                            numMisclassified += 1
                        if goldedEroded[row][col] != 0:
                            numForeground += 1
            # SNR conversion
            den = numMisclassified
            num = numForeground
            if den==0:
                return SNR_MAX
            else:
                snr = 20 * np.log10( num/den )
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
                        den += (goldenData[i][j].real - relaxedData[i][j].real) ** 2
                        den += (goldenData[i][j].imag - relaxedData[i][j].imag) ** 2
                        num += goldenData[i][j].real * goldenData[i][j].real + \
                               goldenData[i][j].imag * goldenData[i][j].imag
                snr = 10 * np.log10( num/den )
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
                snr = 10 * np.log10( num/den )
                return snr
        elif mode=="bin":
            goldenData = load_img_bin(golden)
            relaxedData = load_img_bin(relaxed)
            if (goldenData==relaxedData).all():
                return SNR_MAX
            else:
                # Here we compute the SNR based on the PERFECT doc
                num = ((goldenData) ** 2).sum(axis=None)
                den = ((goldenData - relaxedData) ** 2).sum(axis=None)
                snr = 10 * np.log10( num/den )
                return snr
        else:
            return SNR_MIN
    else:
        return SNR_MIN

def computePSNR(golden, relaxed, mode):
    if (os.path.isfile(relaxed)):
        if mode=="RGBbin":
            goldenData = load_img_bin(golden, luma=True)
            relaxedData = load_img_bin(relaxed, luma=True)
            if (goldenData==relaxedData).all():
                return SNR_MAX
            else:
                # For details on how to compute PSNR in multimedia applications
                # https://en.wikipedia.org/wiki/Peak_signal-to-noise_ratio
                mseVal = ((goldenData - relaxedData) ** 2).mean(axis=None)
                maxVal = np.amax(goldenData)
                psnr = 20 * np.log10(maxVal) - 10 * np.log10(mseVal)
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
        try:
            img_array = load_img_bin(fn, normalize=True)
            print img_array[0]
            channels = 1 if len(img_array.shape)==2 else img_array.shape[2]
            if channels==1:
                plt.imshow(img_array, cmap = cm.Greys_r)
            else:
                plt.imshow(img_array)
            plt.show()
        except:
            img_array = load_wami_img(fn)

            ax1 = plt.subplot(231)
            ax2 = plt.subplot(232)
            ax3 = plt.subplot(233)
            ax4 = plt.subplot(234)
            ax5 = plt.subplot(235)

            ax1.imshow(img_array[0], interpolation='nearest', cmap = cm.Greys_r)
            ax2.imshow(img_array[1], interpolation='nearest', cmap = cm.Greys_r)
            ax3.imshow(img_array[2], interpolation='nearest', cmap = cm.Greys_r)
            ax4.imshow(img_array[3], interpolation='nearest', cmap = cm.Greys_r)
            ax5.imshow(img_array[4], interpolation='nearest', cmap = cm.Greys_r)

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
