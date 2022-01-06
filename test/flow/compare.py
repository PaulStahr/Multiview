import numpy as np
import imageio
import os
from numpy import linalg
import pyexr
import Imath
import sys


def read_image(file):
    img = None
    if file.endswith(".exr"):
        img = pyexr.read(file)
    else:
        img = imageio.imread(file)
    if len(img.shape) == 2:
        img = img[..., None]
    return img


test = read_image(sys.argv[1])
expected = read_image(sys.argv[2])

maxdiff = np.max(np.abs(expected - test))/np.max(expected)
print(maxdiff)
