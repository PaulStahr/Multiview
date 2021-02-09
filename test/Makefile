Multiview:
	cd ../; make all

flow/tmp/forward.exr: ../Multiview
	cd flow; valgrind ../../Multiview flow

flow/tmp/forward_advanced.exr: ../Multiview
	cd flow; valgrind ../../Multiview flow_advanced

test_flow_advanced: flow/tmp/forward_advanced.exr flow/data/forward.exr
	cd flow; python3 compare.py "tmp/forward_advanced.exr" "data/forward.exr"

test_flow: flow/tmp/forward.exr flow/data/forward.exr
	cd flow; python3 compare.py "tmp/forward.exr" "data/forward.exr"
all: test_flow test_flow_advanced
