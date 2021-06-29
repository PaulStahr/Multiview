# Multiview
A program which calculates different optical measures in Opengl.

Compile:
You will need the following librarys:
libopenexr-dev, libpng-dev


For faster loading of wavefront-files clone the following repository into src/fast_float:
https://github.com/fastfloat/fast_float.git


After compiling the program can be started with
./Multiview (<script>)

This will open the gui and run the commands of the script line by line if specified. Control can be done either by command line or by the gui. The most important commands are:

object myobject ${sdir}/../../meshes/sphere.obj pos 0 0 0 scale 1 1 0.5 #Reads a wavefront-file, names it myoobject and performs transformations (from left to right)

id myobject 255  #Assigns an id, which can be rendered into an exported image for further computation

camera mycamera #Creates a new camera

anim "anim.csv" myobject pos myobject rot mycamera pos mycamera rot #each label indicates how to interpret the representing column <object> <rot/pos>, rotations are fetched in quaternions, columns can be skipped by scip <n>

exit #Exit program

help  # Get a longer list of possible commands.

Putting an & at the end of the line results of running the command in background, which can be usefull to increse overall performance but keep in mind, that the program works as a state-machine, making it necessary to synchronize/wait for pending tasks.

For more information please visit the wiki at https://github.com/PaulStahr/Multiview/wiki
