cmake_minimum_required(VERSION 3.0.0)
cmake_policy(SET CMP0091 NEW)
project(mtl VERSION 0.1.0)

enable_language(CXX)
set(CMAKE_MSVC_RUNTIME_LIBRARY "MultiThreaded$<$<CONFIG:Debug>:Debug>DLL")

add_subdirectory(extern/googletest)
add_subdirectory(MTL)
add_subdirectory(mtlgen)

include(CTest)

set(CPACK_PROJECT_NAME ${PROJECT_NAME})
set(CPACK_PROJECT_VERSION ${PROJECT_VERSION})
include(CPack)

install(FILES LICENSE Readme.md TYPE DOC  )
