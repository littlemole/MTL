# CMakeList.txt : CMake project for MTL, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)

#include(GoogleTest)

#enable_testing()
#find_package(GTest REQUIRED)
#include_directories(${GTEST_INCLUDE_DIRS})

#enable_testing()
#find_package(GTest REQUIRED)
#include_directories(${GTEST_INCLUDE_DIRS})

add_definitions(-DUNICODE)
add_definitions(-D_UNICODE)

#file(GLOB TESTSOURCES test/*.cpp)
#add_executable(FooTest ${TESTSOURCES})
 
option(PACKAGE_TESTS "Build the tests" ON)
if(PACKAGE_TESTS)
    enable_testing()
    include(GoogleTest)
    add_subdirectory(tests)
endif()

#
#gtest_add_tests(TARGET      FooTest
#                TEST_SUFFIX .noArgs
#                TEST_LIST   noArgsTests
#)

#set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")
#include_directories(..)
# Add source to this project's executable.

include_directories(include)
#add_library (MTL STATIC "MTL.cpp" "MTL.h" "uni.h" "variant.h" "variant.cpp" "punk.h" "module.h" "fwd.h" "impl.h" "disp.h" "inproc.h" "localserver.h" "targetver.h" "out/aut.h")

# TODO: Add tests and install targets if needed.


install(DIRECTORY include/MTL TYPE INCLUDE)