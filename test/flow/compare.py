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

#Create a grid of coordinates, where each entry states the coordinate of the cells-center. This should match most opengl-implementations
grid = np.meshgrid(np.linspace(-1,1,test.shape[0],endpoint=False)+1/test.shape[0],np.linspace(-1,1,test.shape[1],endpoint=False)+1/test.shape[1])
#Elevation in a spherical equidistant coordinate system ranging from 0 to 1
elev = np.sqrt(grid[0] ** 2 + grid[1] ** 2)
#Expected optic flow is orthogonal to the image-position and has a total magnitude of the sine of elevation
expected = np.dstack((-grid[1],grid[0],grid[0]*0)) * (np.sin(elev * np.pi / 2) / elev)[:,:,None] * 1 * np.pi / 180
#We are only interested in pixels which don't have more that 90 degree elevation
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

#As the implementation currently uses half floats with a mantissa of 10 this should result in an error of about 2^(-10)=0.0009765625
print(maxdiff)
if not maxdiff < 2**(-10)*2:
    raise Exception(maxdiff, "higher than expected", 2^(-10)*2)
