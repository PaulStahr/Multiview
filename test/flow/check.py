import numpy as np
import imageio
import os
from numpy import linalg
import pyexr
import Imath


def read_image(file):
    img = None
    if file.endswith(".exr"):
        img = pyexr.read(file)
    else:
        img = imageio.imread(file)
    if len(img.shape) == 2:
        img = img[..., None]
    return img


test = read_image("tmp/forward.exr")
expected = read_image("data/forward.exr")

maxdiff = np.max(np.abs(expected - test))/np.max(expected)
print(maxdiff)
