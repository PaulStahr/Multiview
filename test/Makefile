../Multiview_debug:
	cd ../; make Multiview_debug

flow/tmp/rotate.exr: ../Multiview_debug flow/flow.mtv flow/trajectory
	-rm flow/tmp/*
	cd flow; LSAN_OPTIONS=suppressions=asan.supp ../../Multiview_debug -s flow.mtv

depth/tmp/sphere.exr: ../Multiview_debug depth/sphere.mtv
	-rm depth/tmp/*
	cd depth; LSAN_OPTIONS=suppressions=asan.supp ../../Multiview_debug -s sphere.mtv

position/tmp/sphere.exr: ../Multiview_debug position/sphere.mtv
	-rm position/tmp/*
	cd position; LSAN_OPTIONS=suppressions=asan.supp ../../Multiview_debug -s sphere.mtv

test_flow: flow/tmp/rotate.exr
	cd flow; python3 compare.py "tmp/rotate.exr"

test_depth: depth/tmp/sphere.exr
	cd depth; python3 compare.py "tmp/sphere.exr"

test_position: position/tmp/sphere.exr
	cd position; python3 compare.py "tmp/sphere.exr"

clean:
	-rm flow/tmp/rotate.exr
	-rm depth/tmp/sphere.exr
	-rm position/tmp/sphere.exr

all: test_flow test_position test_depth
