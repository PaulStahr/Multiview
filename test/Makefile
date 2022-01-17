Multiview:
	cd ../; make all

flow/tmp/rotate.exr: ../Multiview_debug flow/flow.mtv flow/trajectory
	cd flow; ../../Multiview_debug flow.mtv

depth/tmp/sphere.exr: ../Multiview_debug depth/sphere.mtv
	cd depth; ../../Multiview_debug sphere.mtv

position/tmp/sphere.exr: ../Multiview_debug position/sphere.mtv
	cd position; ../../Multiview_debug sphere.mtv

test_flow: flow/tmp/rotate.exr
	cd flow; python3 compare.py "tmp/rotate.exr"

test_depth: depth/tmp/sphere.exr
	cd depth; python3 compare.py "tmp/sphere.exr"

test_position: position/tmp/sphere.exr
	cd position; python3 compare.py "tmp/sphere.exr"

all: test_flow test_position test_depth
