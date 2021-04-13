cmake_minimum_required(VERSION 3.0.0)
project(mtl VERSION 0.1.0)
cmake_policy(SET CMP0091 NEW)
include(CTest)

enable_language(CXX)
#set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>")

# optional tests with embedded googletest

option(DO_RUN_TESTS "Build Library with test" OFF)

if(DO_RUN_TESTS)

if(EXISTS googletest-download/CMakeLists.txt)
else()

configure_file(CMakeLists.txt.in googletest-download/CMakeLists.txt)

execute_process(COMMAND "${CMAKE_COMMAND}" -G "${CMAKE_GENERATOR}" .
    WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/googletest-download"
)

execute_process(COMMAND "${CMAKE_COMMAND}" --build .
    WORKING_DIRECTORY "${CMAKE_BINARY_DIR}/googletest-download"
)
endif()
endif()


# add sub projects

add_subdirectory(MTL)
add_subdirectory(mtlgen)


# install shared doc
install(FILES LICENSE Readme.md TYPE DOC  )


# auto run cpack after build
set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

file(REMOVE_RECURSE ${CMAKE_BINARY_DIR}/_CPack_Packages )

add_custom_command(
     OUTPUT "${CMAKE_BINARY_DIR}/_CPack_Packages.txt"
     COMMENT "Run cpack -G NuGet" 
     POST_BUILD 
     WORKING_DIRECTORY ${CMAKE_BINARY_DIR}
     COMMAND ${CMAKE_CPACK_COMMAND} -G NuGet 
)

add_custom_target(autopack ALL DEPENDS "${CMAKE_BINARY_DIR}/_CPack_Packages.txt")

add_dependencies(autopack MTL mtlgen tests)
