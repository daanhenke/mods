cmake_minimum_required(VERSION 3.4)
project(libmod)

if(WIN32)
    set(ARCH_SOURCES
        arch/win32/uninject.cc
        arch/win32/mem.cc
        arch/win32/hooks/dx11.cc
        arch/win32/hooks/imgui_dx11.cc
    )
endif()

add_library(${PROJECT_NAME}
    ${ARCH_SOURCES}
    app/main.cc
    hooks/vtable.cc
)

if(WIN32)
    target_link_libraries(${PROJECT_NAME} d3d11)
endif()

target_include_directories(${PROJECT_NAME} PUBLIC ${CMAKE_CURRENT_SOURCE_DIR}/include)
