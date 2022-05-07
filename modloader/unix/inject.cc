#include <inject.hh>
#include <unix/processapi_ext.hh>
#include <unix/elf_parser.hh>
#include <unix/fs.hh>
#include <iostream>

#include <cstring>

using namespace std::filesystem;

namespace modloader
{
    template <typename... T>
    void print_ptrs(T... args)
    {
        std::cout << "pointers: " << std::hex << std::endl;
        ((std::cout << args << " "), ...);
        std::cout << std::endl;
    }

    bool inject(std::uintptr_t pid, std::filesystem::path dll_path)
    {
        // Get path to libc.so or bail out
        path libc_path;
        if (! find_shared_object("libc.so", &libc_path)) return false;

        // Get all our necessary function ptrs
        elf64 libc_elf(libc_path);
        if (! libc_elf.is_valid()) return false;

        auto p_base = get_base(pid);
        auto p_dlopen = p_base + libc_elf.get_export_offset("__libc_dlopen_mode");
        auto p_malloc = p_base + libc_elf.get_export_offset("malloc");
        auto p_free = p_base + libc_elf.get_export_offset("free");
        auto p_mmap = p_base + libc_elf.get_export_offset("mmap");
        auto p_munmap = p_base + libc_elf.get_export_offset("munmap");

        print_ptrs(p_base, p_dlopen, p_malloc, p_free, p_mmap, p_munmap);

        // Attach to our process and set up our primitives
        auto ptrace = ptrace_wrapper(pid, p_mmap, p_munmap);
        if (ptrace.has_errored()) return false;

        auto remote_argument = ptrace.execute_function(p_malloc, 0x1337);

        std::cout << "yeeting memory..." << std::endl << std::flush;

        char yeet_in[] = "enge zaken en memecopy spooky";
        ptrace.write_to(ptrace.m_shellcode_address, strlen(yeet_in), yeet_in);

        char yeet_out[1024] = {'A'};
        ptrace.read_from(ptrace.m_shellcode_address, strlen(yeet_in), yeet_out);

        std::cout << yeet_out << std::endl << std::flush;

        return false;
    }
}
