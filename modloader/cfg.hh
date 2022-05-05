#pragma once

#include <string>
#include <map>
#include <cstdint>

namespace modloader::cfg
{
    typedef struct
    {
        bool enabled;
        bool dev_mode;
        std::uint64_t dev_last_updated;
    } target_t;

    typedef struct
    {
        std::map<std::string, target_t> targets;
    } config_t;

    void load();
    config_t* get();
}
