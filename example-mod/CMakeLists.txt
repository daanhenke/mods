add_library(example-mod SHARED main.cc)
target_link_libraries(example-mod libmod)

install(TARGETS
    example-mod
    DESTINATION bin/mods/sleep
)
