# CMakeList.txt : CMake project for mtlgen, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

include_directories("../MTL/include")
# Add source to this project's executable.
add_executable (mtlgen 
	"mtlgen.cpp" "util.cpp" 
	"mtlgen.h" "disp.h" "dllmodule.h" "manifest.h" "packer.h" "Parser.h" "reg.h" "selfreg.h" "util.h" "wix.h"
 "init.h")

# TODO: Add tests and install targets if needed.

install(TARGETS mtlgen RUNTIME)
install(DIRECTORY msbuild DESTINATION share/mtl)