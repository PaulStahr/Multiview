../Multiview_debug:
	cd ../; make Multiview_debug

flow/tmp/rotate.exr: ../Multiview_debug flow/flow.mtv flow/trajectory
	-rm flow/tmp/*
	cd flow; LSAN_OPTIONS=suppressions=asan.supp ../../Multiview_debug -s flow.mtv

depth/tmp/sphere.exr: ../Multiview_debug depth/sphere.mtv
	-rm depth/tmp/*
	cd depth; LSAN_OPTIONS=suppressions=asan.supp ../../Multiview_debug -s sphere.mtv

position/tmp/sphere.exr: ../Multiview_debug position/sphere.mtv
	-rm position/tmp/spere.exr
	cd position; LSAN_OPTIONS=suppressions=asan.supp ../../Multiview_debug -s sphere.mtv

position/tmp/sphere_python.exr: ../Multiview_debug position/sphere.py
	-rm position/tmp/sphere_python.exr
	cd position; LSAN_OPTIONS=suppressions=asan.supp ../../Multiview_debug -p sphere.py

test_python: position/tmp/sphere_python.exr
	cd position; python3 compare.py "tmp/sphere_python.exr"

test_flow: flow/tmp/rotate.exr
	cd flow; python3 compare.py "tmp/rotate.exr"

test_depth: depth/tmp/sphere.exr
	cd depth; python3 compare.py "tmp/sphere.exr"

test_position: position/tmp/sphere.exr
	cd position; python3 compare.py "tmp/sphere.exr"

test_flow_magnitude:
	cd flow; LSAN_OPTIONS=suppressions=asan.supp ../../Multiview_debug -s flow_rotations.mtv;
	cd flow; python3 compare_limits.py --images tmp/pitch.exr tmp/roll.exr tmp/yaw.exr --directions x y
	cd flow; python3 compare_limits.py --images tmp/pitch.exr tmp/roll.exr tmp/yaw.exr --directions x y z
	rm -r flow/tmp/pitch.exr flow/tmp/roll.exr flow/tmp/yaw.exr

clean:
	-rm flow/tmp/rotate.exr
	-rm depth/tmp/sphere.exr
	-rm position/tmp/sphere.exr

all: test_flow test_position test_depth test_python
