import Multiview as mv
import sys
import os
import time

mv.play = 0
mv.preresolution = 1024
mv.crop = False
mv.exec('object env "../meshes/sphere.obj"',mv.sarray([]),env,session)
mv.animating = mv.RedrawScedule.redraw_manual
mv.frame = 1
session.scene.add_camera("cam")
mv.screenshot(env, session, "tmp/sphere_python.exr", "cam", mv.Viewtype.position, 128, 128, mv.sarray([]), False, True)
env.join(None, mv.PendingFlag.scene_edit)
session.update_session(mv.SessionUpdateType.redraw)
env.join(None, mv.PendingFlag.texture_read)
env.join(None, mv.PendingFlag.all)
time.sleep(0.1)
session.exit()
