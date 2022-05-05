#include <processapi.hh>

namespace modloader
{
    std::filesystem::path get_current_module_dir()
    {
        return std::filesystem::path();
    }

    std::vector<process_info_t> get_processes()
    {
        auto result = std::vector<process_info_t>();

        return result;
    }

    bool is_injectable_file(std::filesystem::path file)
    {
        return false;
    }
}
