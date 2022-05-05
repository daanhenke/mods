#pragma once
#include <sstream>
#include <string>
#include <fstream>
#include <filesystem>
#include <unistd.h>

namespace modloader
{
    uintptr_t get_base(uintptr_t pid, std::string target_module_name = "libc");
    uintptr_t translate_pointer(uintptr_t pid, uintptr_t source_pointer, std::string target_module_name = "libc");
}
