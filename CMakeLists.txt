cmake_minimum_required(VERSION 3.20)
project(sndrecv_examples)

set(CMAKE_CXX_STANDARD 20)

include(ExternalProject)
ExternalProject_Add(libunifex_project
        GIT_REPOSITORY https://github.com/facebookexperimental/libunifex.git
        GIT_TAG 67829c4
        SOURCE_DIR "${CMAKE_CURRENT_SOURCE_DIR}/external/libunifex"
        BINARY_DIR "${CMAKE_CURRENT_BINARY_DIR}/external/libunifex"
        )

set(libunifex_includes
        "${CMAKE_CURRENT_SOURCE_DIR}/external/libunifex/include"
        "${CMAKE_CURRENT_BINARY_DIR}/external/libunifex/include"
        )
add_library(libunifex STATIC IMPORTED)
#add_dependencies(libunifex libunifex_project)
set_property(TARGET libunifex PROPERTY IMPORTED_LOCATION ${CMAKE_CURRENT_BINARY_DIR}/external/libunifex/source/libunifex.a)
target_include_directories(libunifex INTERFACE ${libunifex_includes})


function(def_example target sourceFile)
    add_executable(${target} ${sourceFile})
    target_link_libraries(${target} libunifex)
endfunction()

def_example(hello "hello.cpp")
def_example(async_start "various/async_start.cpp")

