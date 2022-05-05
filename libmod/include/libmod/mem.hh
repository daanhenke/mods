#pragma once

#include <cstdint>

namespace mod::mem
{
    std::uint32_t set_protection_raw(std::uintptr_t page, std::uint32_t protection, std::uint32_t page_count);
    std::uint32_t set_rw(std::uintptr_t page, std::uint32_t page_count);

    bool is_code_ptr(std::uintptr_t pointer);
}
