#include <libmod/hooks.hh>
#include <libmod/mem.hh>

#include <string>

std::uint32_t get_vtable_size(std::uintptr_t* vtable_ptr)
{
    std::uint32_t size = 0;
    while (mod::mem::is_code_ptr(vtable_ptr[size])) size++;
    return size;
}

namespace mod::hooks
{
    vtable_hook_t::vtable_hook_t(uintptr_t* vtable_ptr)
    {
        m_vtable = vtable_ptr;
    };

    void vtable_hook_t::swap()
    {
        for (auto& pair : m_changed_entries)
        {
            auto index = pair.first;
            auto hook = pair.second;
            auto vtable_entry_addr = reinterpret_cast<uintptr_t>(&(m_vtable[index]));

            auto prev_protection = mem::set_rw(vtable_entry_addr, 1);
            m_changed_entries[index] = m_vtable[index];
            m_vtable[index] = hook;
            mem::set_protection_raw(vtable_entry_addr, prev_protection, 1);
        }
    }

    void vtable_hook_t::enable()
    {
        swap();
    }

    void vtable_hook_t::disable()
    {
        swap();
    }

    std::uintptr_t vtable_hook_t::overwrite_entry(std::uint32_t index, std::uintptr_t pointer)
    {
        m_changed_entries[index] = pointer;
        return m_vtable[index];
    }
}
