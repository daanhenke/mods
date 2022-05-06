#pragma once

#include <filesystem>

namespace modloader
{
    bool find_shared_object(std::string partial_name, std::filesystem::path* result);
}
