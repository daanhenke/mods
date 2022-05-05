#pragma once

#include <cstdint>
#include <map>
#include <string>
#include <chrono>
#include <filesystem>

namespace modloader::app
{
    void init();
    void exit();
    bool is_running();

    typedef struct {
        std::map<uint64_t, std::map<std::string, std::filesystem::file_time_type>> pid_mod_time_map;
    } injection_ctx_t;
}
