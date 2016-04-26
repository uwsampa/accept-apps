from __future__ import division
import itertools
import png
import csv
import numpy as np

def rgbload(file):
    csvReader = csv.reader(open(file, 'r'), delimiter=',', quotechar='"')

    i = 0
    pixels = []
    width = 0
    height = 0
    meta = {}
    for row in csvReader:
        if (i == 0):
            width = int(row[0])
            height = int(row[1])
        elif (i == height + 1):
            meta = row[0]
            break
        else:
            row = [255 if int(e) > 255 else int(e) for e in row]
            row = [0 if int(e) < 0 else int(e) for e in row]
            pixels.append(tuple(row))

        i = i + 1

    return(width, height, pixels, meta)

def rgb2png(img, file):
    f = open(file, 'wb')
    pngWriter = png.Writer(img[0], img[1])
    pngWriter.write(f, img[2])
    f.close()

def score(orig, relaxed):
    goldenData = np.array(rgbload(orig)[2])
    relaxedData = np.array(rgbload(relaxed)[2])

    num = ((goldenData) ** 2).sum(axis=None)
    den = ((goldenData - relaxedData) ** 2).sum(axis=None)
    snr = 10 * np.log10( num/den )
    return snr

if __name__ == '__main__':
    # Print the SNR
    print score("orig.rgb", "out.rgb")
    # Convert the out.rgb and orig.rgb to png for viewing
    rgb2png(rgbload("out.rgb"), "out.png")
    rgb2png(rgbload("orig.rgb"), "orig.png")
