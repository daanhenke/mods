add_library(nier-automata-sdk INTERFACE)
target_include_directories(nier-automata-sdk INTERFACE sdk)

add_library(nier-automata-retool SHARED retool/main.cc)
target_link_libraries(nier-automata-retool libmod zasm nier-automata-sdk)

install(TARGETS
    nier-automata-retool
    DESTINATION bin/mods/nierautomata.exe
)
