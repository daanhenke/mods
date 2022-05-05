#include <libmod/mem.hh>
#include <Windows.h>

namespace mod::mem
{
    std::uint32_t set_protection_raw(std::uintptr_t page, std::uint32_t protection, std::uint32_t page_count)
    {
        std::uint32_t old_protection;
        VirtualProtect(reinterpret_cast<void*>(page & (~0xFFF)), 0x1000 * page_count, protection, reinterpret_cast<PDWORD>(&old_protection));
        return old_protection;
    }

    std::uint32_t set_rw(std::uintptr_t page, std::uint32_t page_count)
    {
        return set_protection_raw(page, PAGE_READWRITE, page_count);
    }

    bool is_code_ptr(std::uintptr_t pointer)
    {
        constexpr DWORD flags = PAGE_EXECUTE | PAGE_EXECUTE_READ | PAGE_EXECUTE_READWRITE | PAGE_EXECUTE_WRITECOPY;

        MEMORY_BASIC_INFORMATION info;
        VirtualQuery(reinterpret_cast<LPCVOID>(pointer), &info, sizeof(info));
        return info.Type && !(info.Protect & (PAGE_GUARD | PAGE_NOACCESS)) && info.Protect & flags;
    }
}
