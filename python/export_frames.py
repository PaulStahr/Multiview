#Example script to create a Multiview-script exporting a bunch of frames either given by a range or by a framelist
#Run python3 export_frames.py <start> <end> <output_folder> <cameras> <passes>
#You can save the output of this file into a textfile and run Multiview later or call it directy from multiview with
#exec python3 export_frames.py

import sys
import numpy as np

def read_numbers(filename):
    with open(filename,'r') as f:
         return np.asarray([int(x) for x in f])
print("play 0")
print("preresolution 1024")
#print("smoothing 2\n")
res = 1024
print("echo ",sys.argv)
output_folder = sys.argv[3]
print("animating manual")
print("autouiupdate 0")
if sys.argv[1] == "framelist":
    framenumbers = read_numbers(sys.argv[2])
elif sys.argv[1] == "mframelist":
    framenumbers = read_numbers(sys.argv[2]) - 1
else:
    framenumbers=range(int(sys.argv[1]), int(sys.argv[2]))
cameras={*sys.argv[4].split("|")}
passes={*sys.argv[5].split("|")}
for i in framenumbers:
    print("frame {0}".format(i))
    if "rendered" in passes:
        for cam in cameras:
            print("screenshot2 \"{2}/rendered/{3}/{0}.png\" {1} {1} {3} rendered 0&\n".format(i,res,output_folder,cam))
    if "index" in passes:
        for cam in cameras:
            print("screenshot2 \"{2}/index/{3}/{0}.png\" {1} {1} {3} index 0&".format(i,res,output_folder,cam))
    if "position" in passes:
        for cam in cameras:
            print("screenshot2 \"{2}/position/{3}/{0}.exr\" {1} {1} {3} position 0&\n".format(i,res,output_folder,cam))
    if "flow" in passes:
        for cam in cameras:
            print("screenshot2 \"{2}/flow/{3}/{0}.exr\" {1} {1} {3} flow 0&\n".format(i,res,output_folder,cam))
    if "depth" in passes:
        for cam in cameras:
            print("screenshot2 \"{2}/depth/{3}/{0}.exr\" {1} {1} {3} depth 0&\n".format(i,res,output_folder,cam))
    if "visibility" in passes:
        for cam in cameras:
            print("screenshot2 \"{2}/visibility/{3}/{0}.png\" {1} {1} {3} rendered 0 vcam {4} vcam {5}&\n".format(i,res,output_folder,cam,cameras[0],cameras[1]))
    print("join swrite")
    print("redraw")
    print("join sread")
print("join")
print("sleep 10000")
