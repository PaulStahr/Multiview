all: Makefile_Release Makefile_Test
	$(MAKE) -f Makefile_Release
	$(MAKE) -f Makefile_Test
	$(MAKE) -f Makefile_Debug
	./unit_test

test: Makefile_Test
	$(MAKE) -f Makefile_Test
	./unit_test

Makefile_Release: Multiview.pro
	qmake -o $@ CONFIG+=Release

Makefile_Debug: Multiview.pro
	qmake -o $@ CONFIG+=Debug

Makefile_Test: Multiview.pro
	qmake -o $@ CONFIG+=Test
