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

    class ptrace_wrapper
    {
    public:
        ptrace_wrapper(uintptr_t pid);
        ~ptrace_wrapper();

        void write_to(uintptr_t address, int size);
        void read_from(uintptr_t address, int size, void* out_buffer);
    }
}
