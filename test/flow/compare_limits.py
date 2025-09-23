import numpy as np
import imageio
import pyexr
import argparse
import logging
from matplotlib import pyplot as plt

#Checks if given image is a 1 degree rotation around z-axis
logger = logging.getLogger(__name__)

def read_image(file):
    if file.endswith(".exr"):
        img = pyexr.read(file)
    else:
        img = imageio.imread(file)
    if len(img.shape) == 2:
        img = img[..., None]
    return img

if __name__ == "__main__":
    parser = argparse.ArgumentParser(description='Compare multiple images for their limits.')
    parser.add_argument('--images', metavar='image', type=str, nargs='+',
                        help='Images to compare')
    parser.add_argument('--show-plots', action='store_true', help='Show plots of the images')
    parser.add_argument('--loglevel', type=str, default="INFO", help="Logging level (DEBUG, I"
                                                                     "NFO, WARN, ERROR)")
    parser.add_argument('--directions', type=str, nargs='+', default=['x', 'y', 'z'], choices=['x', 'y', 'z'],)
    args = parser.parse_args()

    logging.basicConfig(level=args.loglevel.upper(), format="%(asctime)s - %(name)s - %(levelname)s - %(message)s")
    #calculate max and minimum of each image
    minima = []
    maxima = []

    indices = [0 if d == 'x' else 1 if d == 'y' else 2 for d in args.directions]

    images = [read_image(file)[...,indices] for file in args.images]

    for imagefile, img in zip(args.images, images):
        magnitude = np.linalg.norm(img, axis=-1)
        minima.append(np.min(magnitude))
        maxima.append(np.max(magnitude))
        logger.info(f"Image {imagefile} has min {minima[-1]} and max {maxima[-1]}")

    if args.show_plots:
        fig, axs = plt.subplots(1, len(args.images), figsize=(5 * len(args.images), 5))
        for i, img in enumerate(images):
            magnitude = np.linalg.norm(img, axis=-1)
            axs[i].imshow(magnitude, vmin=0, vmax=np.max(maxima))
            axs[i].set_title(f"Image {args.images[i]}\nmin: {minima[i]:.4f}, max: {maxima[i]:.4f}")
        plt.show()

    np.testing.assert_allclose(minima, minima[0], rtol=1e-4, atol=1e-8)
    np.testing.assert_allclose(maxima, maxima[0], rtol=1e-4, atol=1e-8)
