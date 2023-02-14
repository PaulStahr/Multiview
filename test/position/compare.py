import numpy as np
import imageio
import os
from numpy import linalg
import pyexr
import sys

#Checks if given image encodes the positions on a sphere
def divlim(divident, divisor):
    return np.divide(divident, divisor, np.zeros_like(divident), where=np.logical_or(divident!=0,divisor!=0))

def equi2cart(x, y):
    radius = np.sqrt(x ** 2 + y ** 2)
    radian = radius * np.pi
    sin = np.sin(radian)
    return np.asarray((divlim(sin * x, radius), divlim(sin * y, radius), np.cos(radian)))

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
expected = np.dstack(equi2cart(*np.asarray(grid)*0.5))
expected[:,:,1]=-expected[:,:,1]
expected[:,:,2]=-expected[:,:,2]
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

print(maxdiff)
if not maxdiff < 0.005:
    raise Exception(maxdiff, "higher than expected", 0.005)
