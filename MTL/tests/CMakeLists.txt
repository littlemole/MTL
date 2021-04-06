# CMakeList.txt : CMake project for mtlgen, include source and define
# project specific logic here.
#
cmake_minimum_required (VERSION 3.8)



link_directories(${PROJECT_SOURCE_DIR}/lib/lib/)
# Add source to this project's executable.
add_executable (tests 
	"test.cpp" 
 "variant.cpp" "collection.cpp" "test.rc")

set(MIDL_OUTPUT
    ${CMAKE_CURRENT_BINARY_DIR}/IFace.h
    ${CMAKE_CURRENT_BINARY_DIR}/GUIDS.c
    ${CMAKE_CURRENT_BINARY_DIR}/Proxy.c
    ${CMAKE_CURRENT_BINARY_DIR}/ProxyDll.c
    )
set(MIDL_FILE
    ${CMAKE_CURRENT_LIST_DIR}/test.idl
    )
add_custom_command(
    OUTPUT ${MIDL_OUTPUT}
    COMMAND midl /h IFace.h /iid GUIDS.c /proxy Proxy.c /dlldata ProxyDll.c ${MIDL_FILE}
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    DEPENDS ${MIDL_FILE}
    VERBATIM
    )
add_custom_target(midl-cmplr
    DEPENDS ${MIDL_OUTPUT}
    )
add_dependencies(tests
    midl-cmplr
    )
set_source_files_properties(
    ${MIDL_OUTPUT}
        PROPERTIES
            GENERATED TRUE
            )




target_include_directories(tests PRIVATE "${PROJECT_SOURCE_DIR}/lib/include")
target_include_directories(tests PRIVATE "${PROJECT_SOURCE_DIR}/MTL/include")
target_include_directories(tests PRIVATE "${CMAKE_CURRENT_BINARY_DIR}")

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
     COMMAND ${CMAKE_CTEST_COMMAND} -C $<CONFIGURATION>  --output-on-failures
)
