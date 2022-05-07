cmake_minimum_required(VERSION 3.4)
project(modding)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

set(CMAKE_INSTALL_PREFIX ${CMAKE_CURRENT_BINARY_DIR}/out)

if(UNIX)
    set(CMAKE_STATIC_LIBRARY_PREFIX "")
endif()

set(ZASM_BUILD_TESTS OFF)
set(ZASM_BUILD_BENCHMARKS OFF)
add_subdirectory(extern/zasm)
add_subdirectory(extern/mini)

add_subdirectory(modloader)
add_subdirectory(libmod)

add_subdirectory(example-mod)

# Windows onlu
if(WIN32)
    add_subdirectory(2bshell)

    # 64 bit only
    if("${CMAKE_SIZEOF_VOID_P}" STREQUAL "8")
        add_subdirectory(nierautomata)
    endif()
endif()
