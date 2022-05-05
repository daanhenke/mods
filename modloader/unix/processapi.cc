#include <processapi.hh>
#include <unix/processapi_ext.hh>

using namespace std::filesystem;

namespace modloader
{
    std::filesystem::path get_current_module_dir()
    {
        return read_symlink("/proc/self/exe").parent_path();
    }

    std::vector<process_info_t> get_processes()
    {
        auto result = std::vector<process_info_t>();

        for(auto exe : directory_iterator("/proc"))
        {
            auto pidpath = exe.path().filename();
            auto pidstring = pidpath.string();
            // check if pidpath is not a number
            if(pidstring.find_first_not_of("0123456789") != std::string::npos)
            {
                continue;
            }
            auto pid = stoul(pidstring);
            try {
                auto process_path = read_symlink("/proc" / pidpath / "exe");
                result.push_back({process_path.filename().string(), pid});
            } catch(...) {
                continue;
            }
        }

        return result;
    }

    std::vector<std::string> injectable_extensions = { ".so", ".elf" };
    bool is_injectable_file(path file)
    {
        if(std::find(injectable_extensions.begin(), injectable_extensions.end(), file.extension().string()) == injectable_extensions.end())
        {
            return false;
        }

        perms p = status(file).permissions();
        return ((p & perms::owner_exec) != perms::none) ||
               ((p & perms::group_exec) != perms::none) ||
               ((p & perms::others_exec) != perms::none);
    }

    uintptr_t get_base(uintptr_t pid, std::string target_module_name)
    {
        std::ifstream infile("/proc/" + std::to_string(pid) + "/maps");
        std::string line;
        while(std::getline(infile, line))
        {
            std::istringstream input_line(line);
            std::string base_end;
            input_line >> base_end;
            for(int tmp = 0; tmp < 4; tmp++)
            {
                std::string _tmp;
                input_line >> _tmp;
            }
            path module_path;
            input_line >> module_path;

            if(module_path.stem().string().starts_with(target_module_name))
            {
                std::string first_value = base_end.substr(0, base_end.find("-"));
                return strtoul(first_value.c_str(), nullptr, 16);
            }
        }

        return 0;
    }

    uintptr_t translate_pointer(uintptr_t pid, uintptr_t source_pointer, std::string target_module_name)
    {
        return source_pointer - get_base(getpid(), target_module_name) + get_base(pid, target_module_name);
    }
}
