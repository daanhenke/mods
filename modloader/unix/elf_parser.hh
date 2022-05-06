#pragma once

#include <cstdint>
#include <filesystem>
#include <elf.h>
#include <fstream>
#include <string>

namespace modloader
{
    class elf64
    {
    public:
        elf64(std::filesystem::path elf_path);
        ~elf64();

        bool is_valid();
        std::uintptr_t get_export_offset(std::string export_name);

        char* get_section_data(int index);

    protected:
        bool m_is_valid;
        std::ifstream m_elf_stream;
        Elf64_Ehdr m_e_hdr;
        Elf64_Shdr* m_s_hdrs;
    };
}
