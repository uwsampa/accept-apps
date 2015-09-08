from __future__ import division
from math import sqrt
import PIL
import PIL.Image
import itertools

IMGDIR = 'saved_outputs'
EXT = '.jpg'

def load():
    return 'file:lena.rgb.jpg'

def score(orig, relaxed):
    # print 'Orig: %s' % orig
    orig_image = PIL.Image.open(orig)
    # print 'Relaxed: %s' % relaxed
    try:
        relaxed_image = PIL.Image.open(relaxed)
    except IOError:
        # Broken JPEG
        return 1.0

    try:
        orig_data = orig_image.getdata()
        relaxed_data = relaxed_image.getdata()
    except ValueError:
        return 1.0

    error = 0
    total = 0
    for ppixel, apixel in itertools.izip(orig_data, relaxed_data):
        # root-mean-square error per pixel
        error += ((ppixel-apixel)/256)**2
        total += 1
    return error / total
