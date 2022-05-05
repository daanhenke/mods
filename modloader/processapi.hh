#pragma once

#include <string>
#include <vector>
#include <cstdint>
#include <filesystem>

namespace modloader
{
    typedef struct
    {
        std::string exe_name;
        std::uintptr_t pid;
    } process_info_t;

    std::filesystem::path get_current_module_dir();
    std::vector<process_info_t> get_processes();
    bool is_injectable_file(std::filesystem::path file);
}
