set(EXECUTABLE_TYPE)

set(CMAKE_CXX_STANDARD 20)
set(CMAKE_CXX_STANDARD_REQUIRED TRUE)

if(WIN32)
    set(ARCH_SOURCES
        win32/main.cc
        win32/processapi.cc
        win32/shellicon_ui.cc
        win32/resources.rc
        win32/inject.cc
    )
    set(EXECUTABLE_TYPE WIN32)
elseif(UNIX)
    set(ARCH_SOURCES
        unix/main.cc
        unix/processapi.cc
        unix/inject.cc
        unix/fs.cc
        unix/elf_parser.cc
        stdout_ui.cc
    )
else()
    message(ERROR "Platform unsupported rn :(")
endif()

add_executable(modloader ${EXECUTABLE_TYPE}
    ${ARCH_SOURCES}
    app.cc
    cfg.cc
)

target_include_directories(modloader PRIVATE ${CMAKE_CURRENT_SOURCE_DIR})
target_link_libraries(modloader mini)

if(UNIX)
    target_link_libraries(modloader zasm)
endif()

install(TARGETS modloader DESTINATION bin)
install(FILES config.ini DESTINATION bin)
