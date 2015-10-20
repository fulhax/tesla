.PHONY: all clean
.SILENT:

all:
	@if [ ! -d build ]; then mkdir build; fi;
	@cd build; cmake ../src -DCMAKE_EXPORT_COMPILE_COMMANDS=1; make
	@if [ -a build/game ]; then mv build/game bin/game; fi;

clean: 
	@if [ -d build ]; then rm -rf build; fi;
	@if [ -a bin/game ]; then rm bin/game; fi;

