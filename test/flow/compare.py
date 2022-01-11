import numpy as np
import imageio
import os
from numpy import linalg
import pyexr
import Imath
import sys

#Checks if given image is a 1 degree rotation around z-axis

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

grid = np.meshgrid(np.linspace(-1,1,test.shape[0]),np.linspace(-1,1,test.shape[1]))
elev = np.sqrt(grid[0] ** 2 + grid[1] ** 2)
expected = np.dstack((-grid[1],grid[0],grid[0]*0)) * (np.sin(elev * np.pi / 2) / elev)[:,:,None] * 1 * np.pi / 180
mask = elev < 1
expected = expected * mask[:,:,None]
test = test * mask[:,:,None]
maxdiff = np.max(np.abs(expected - test))/np.max(expected)

if (False):
	import matplotlib.pyplot as plt
	plt.rcParams['figure.figsize'] = [20, 15]
	fig, axs = plt.subplots(3)
	axs[0].imshow(expected * 2 + 0.5)
	axs[1].imshow(test * 2 + 0.5)
	axs[2].imshow((expected - test) * 10 + 0.5)
	plt.show()

print(maxdiff)
