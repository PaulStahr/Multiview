flow/tmp/forward.exr: ../Multiview
	cd flow; ../../Multiview flow

test_flow: flow/tmp/forward.exr flow/data/forward.exr
	cd flow; python3 check.py
all: test_flow
