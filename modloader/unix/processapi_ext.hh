#pragma once
#include <sstream>
#include <string>
#include <fstream>
#include <filesystem>
#include <unistd.h>
#include <sys/user.h>

namespace modloader
{
    std::uintptr_t get_base(std::uintptr_t pid, std::string target_module_name = "libc");
    std::uintptr_t translate_pointer(std::uintptr_t pid, std::uintptr_t source_pointer, std::string target_module_name = "libc");

    class ptrace_wrapper
    {
    public:
        ptrace_wrapper(std::uintptr_t pid, std::uintptr_t p_mmap, std::uintptr_t p_munmap);
        ~ptrace_wrapper();

        bool has_errored();

        bool write_to(std::uintptr_t address, int size, char* in_buffer);
        void read_from(std::uintptr_t address, int size, char* out_buffer);

        std::uintptr_t execute_single_syscall(std::uintptr_t address, std::uintptr_t arg1 = 0, std::uintptr_t arg2 = 0, std::uintptr_t arg3 = 0, std::uintptr_t arg4 = 0, std::uintptr_t arg5 = 0, std::uintptr_t arg6 = 0);
        std::uintptr_t execute_function(std::uintptr_t address, std::uintptr_t arg1); // maybe more args idk, need 1 for dlopen free and malloc

        std::uintptr_t m_shellcode_address;
    protected:
        bool m_has_errored;

        std::uintptr_t m_pid;

        std::uintptr_t m_p_mmap;
        std::uintptr_t m_p_munmap;

        int m_shellcode_size;

        struct user_regs_struct m_regs;
        struct user_regs_struct m_original_regs;
    };
}
