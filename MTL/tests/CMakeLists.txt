# CMakeList.txt : CMake project for mtlgen, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)



link_directories(${PROJECT_SOURCE_DIR}/out/build/x64-Debug/lib)

# Add source to this project's executable.
add_executable (tests 
	"test.cpp" 
)


target_include_directories(tests PRIVATE "../../extern/googletest/googletest/include")

target_link_libraries(tests gtestd )#gtest_maind)
gtest_add_tests(TARGET      tests
                TEST_SUFFIX .noArgs
                TEST_LIST   noArgsTests
)

# TODO: Add tests and install targets if needed.

add_custom_command(
     TARGET tests
     COMMENT "Run tests"
     POST_BUILD 
     WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
     COMMAND ${CMAKE_CTEST_COMMAND} -C $<CONFIGURATION> -R "^test$" --output-on-failures
)
