Multiview:
	cd ../; make all

flow/tmp/rotate.exr: ../Multiview_debug flow/flow.mtv flow/trajectory
	cd flow; ../../Multiview_debug flow.mtv

flow/tmp/forward_advanced.exr: ../Multiview
	cd flow; valgrind ../../Multiview flow_advanced

test_flow_advanced: flow/tmp/forward_advanced.exr flow/data/forward.exr
	cd flow; python3 compare.py "tmp/forward_advanced.exr" "data/forward.exr"

test_flow: flow/tmp/rotate.exr
	cd flow; python3 compare.py "tmp/rotate.exr"
all: test_flow test_flow_advanced
