all: Makefile_Release Makefile_Test
	$(MAKE) -f Makefile_Release
	$(MAKE) -f Makefile_Test

Makefile_Release: Multiview.pro
	qmake -o $@ CONFIG+=Release

Makefile_Test: Multiview.pro
	qmake -o $@ CONFIG+=Test
