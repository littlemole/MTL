cmake_minimum_required (VERSION 3.8)


if(DO_RUN_TESTS)
    enable_testing()
    include(GoogleTest)
    add_subdirectory(tests)
endif()

add_library(MTL INTERFACE)

install(DIRECTORY include/MTL TYPE INCLUDE)


