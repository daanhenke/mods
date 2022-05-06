#include <unix/elf_parser.hh>
#include <cstring>

namespace modloader
{
    const char elf_magic[] = { 0x7f, 'E', 'L', 'F', '\0' };
    elf64::elf64(std::filesystem::path elf_path)
    {
        m_is_valid = true;

        m_elf_stream = std::ifstream(elf_path);
        m_elf_stream.read(reinterpret_cast<char*>(&m_e_hdr), sizeof m_e_hdr);

        // Make sure the magic is there
        if (std::strncmp(reinterpret_cast<const char*>(m_e_hdr.e_ident), elf_magic, std::strlen(elf_magic)) != 0)
        {
            m_is_valid = false;
            return;
        }

        // Find section headers
        m_s_hdrs = new Elf64_Shdr[m_e_hdr.e_shnum];
        m_elf_stream.seekg(m_e_hdr.e_shoff, std::fstream::beg);
        m_elf_stream.read(reinterpret_cast<char*>(m_s_hdrs), m_e_hdr.e_shentsize * m_e_hdr.e_shnum);
    }

    elf64::~elf64()
    {
        delete m_s_hdrs;
        m_elf_stream.close();
    }

    std::uintptr_t elf64::get_export_offset(std::string export_name)
    {
        // Go over each section and check if it's a symbol table
        for (auto i = 0; i < m_e_hdr.e_shnum; i++)
        {
            auto& hdr = m_s_hdrs[i];
            if ((hdr.sh_type == SHT_SYMTAB) || (hdr.sh_type == SHT_DYNSYM))
            {
                // Read the symtab and it's associated names section
                auto sym_section = reinterpret_cast<Elf64_Sym*>(get_section_data(i));
                auto sym_count = hdr.sh_size / sizeof(Elf64_Sym);
                auto text_section = get_section_data(hdr.sh_link);

                // Iterate over every symbol
                for (auto sym_i = 0; sym_i < sym_count; sym_i++)
                {
                    auto& sym = sym_section[sym_i];
                    auto sym_name = text_section + sym.st_name;

                    if (sym_name == export_name)
                    {
                        auto value = sym.st_value;
                        delete sym_section;
                        delete text_section;
                        return value;
                    }
                }

                delete sym_section;
                delete text_section;
            }
        }
        return 0;
    }

    bool elf64::is_valid()
    {
        return m_is_valid;
    }

    char* elf64::get_section_data(int index)
    {
        auto& hdr = m_s_hdrs[index];
        auto result = new char[hdr.sh_size];

        m_elf_stream.seekg(hdr.sh_offset, std::ifstream::beg);
        m_elf_stream.read(result, hdr.sh_size);

        return result;
    }
}
