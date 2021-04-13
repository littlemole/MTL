cmake_minimum_required (VERSION 3.8)

message("-----------------------------")
message("${CMAKE_CURRENT_BINARY_DIR}/..")
message("${PROJECT_BINARY_DIR}/..")
message("-----------------------------")

# cconfigure tests
add_executable (tests )

target_sources(tests PRIVATE 
    "test.cpp" "variant.cpp" "collection.cpp" 
    "test.rc" "test.manifest" 
    "${CMAKE_CURRENT_BINARY_DIR}/GUIDS.c"
)

target_compile_definitions(tests PRIVATE UNICODE _UNICODE)
target_include_directories(tests PRIVATE "${PROJECT_SOURCE_DIR}/lib/include")
target_include_directories(tests PRIVATE "${PROJECT_SOURCE_DIR}/MTL/include")
target_include_directories(tests PRIVATE "${CMAKE_CURRENT_BINARY_DIR}")
target_link_directories(tests PRIVATE ${PROJECT_SOURCE_DIR}/lib/lib/)
target_link_libraries(tests gtestd )
add_dependencies(tests midl-cmplr)

gtest_add_tests(TARGET tests TEST_SUFFIX .noArgs TEST_LIST noArgsTests)

# configure MIDL compiler
set(MIDL_OUTPUT
    ${CMAKE_CURRENT_BINARY_DIR}/IFace.h
    ${CMAKE_CURRENT_BINARY_DIR}/GUIDS.c
    ${CMAKE_CURRENT_BINARY_DIR}/Proxy.c
    ${CMAKE_CURRENT_BINARY_DIR}/ProxyDll.c
)

set(MIDL_FILE ${CMAKE_CURRENT_LIST_DIR}/test.idl )

add_custom_command(
    OUTPUT ${MIDL_OUTPUT}
    COMMAND midl /h IFace.h /iid GUIDS.c /proxy Proxy.c /dlldata ProxyDll.c ${MIDL_FILE}
    WORKING_DIRECTORY ${CMAKE_CURRENT_BINARY_DIR}
    DEPENDS ${MIDL_FILE}
    VERBATIM
)

add_custom_target(midl-cmplr DEPENDS ${MIDL_OUTPUT} )

set_source_files_properties( ${MIDL_OUTPUT} PROPERTIES GENERATED TRUE )

# execute tests after successful build

add_custom_command(
     TARGET tests
     COMMENT "Run tests"
     POST_BUILD 
     WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
     COMMAND ${CMAKE_CTEST_COMMAND} -C $<CONFIGURATION>  --output-on-failures
)
