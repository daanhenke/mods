#include <processapi.hh>
#include <ui.hh>

#include <string>
#include <algorithm>
#include <cctype>

#include <Windows.h>
#include <TlHelp32.h>

namespace modloader
{
    std::filesystem::path get_current_module_dir()
    {
        char buffer[MAX_PATH + 1];
        GetModuleFileNameA(GetModuleHandleA(nullptr), reinterpret_cast<LPSTR>(&buffer), MAX_PATH);
        buffer[MAX_PATH] = '\0';

        return std::filesystem::path(buffer).parent_path();
    }

    std::vector<process_info_t> get_processes()
    {
        std::vector<process_info_t> result;

        auto snapshot = CreateToolhelp32Snapshot(TH32CS_SNAPPROCESS, 0);
        if (snapshot != INVALID_HANDLE_VALUE)
        {
            PROCESSENTRY32 pe32;
            pe32.dwSize = sizeof(pe32);

            if (Process32First(snapshot, &pe32))
            {
                do
                {
                    auto name = std::string(pe32.szExeFile);
                    std::transform(name.begin(), name.end(), name.begin(), [](unsigned char c) { return std::tolower(c); });

                    result.push_back(process_info_t { name, pe32.th32ProcessID });
                }
                while (Process32Next(snapshot, &pe32));
            }
            CloseHandle(snapshot);
        }
        return result;
    }

    std::vector<std::string> injectable_extensions = { ".dll", ".exe" };
    bool is_injectable_file(std::filesystem::path file)
    {
        return std::find(injectable_extensions.begin(), injectable_extensions.end(), file.extension().string()) != injectable_extensions.end();
    }
}
