#pragma

#include <cstdint>
#include <filesystem>

namespace modloader
{
    bool inject(std::uintptr_t pid, std::filesystem::path dll_path);
}
