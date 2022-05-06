#include <inject.hh>
#include <unix/processapi_ext.hh>
#include <unix/elf_parser.hh>
#include <unix/fs.hh>
#include <iostream>

using namespace std::filesystem;

namespace modloader
{
    bool inject(std::uintptr_t pid, std::filesystem::path dll_path)
    {
        // Get path to libc.so or bail out
        path libc_path;
        if (! find_shared_object("libc.so", &libc_path)) return false;

        elf64 libc_elf(libc_path);
        if (! libc_elf.is_valid()) return false;

        auto fuckyou =  libc_elf.get_export_offset("__libc_dlopen_mode");

        std::cout << "cool: " << std::hex << fuckyou << std::endl;
        // auto remote = translate_pointer(pid, local);

        return false;
    }
}
