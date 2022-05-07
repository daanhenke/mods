#include <processapi.hh>
#include <unix/processapi_ext.hh>
#include <sys/mman.h>
#include <sys/ptrace.h>
#include <sys/wait.h>
#include <cstring>

#include <iostream>

#define ptrace_succeeded(status_code) (status_code != -1)
#define wait(pid) (waitpid(pid, &g_waitpid_result, WUNTRACED) == pid)

using namespace std::filesystem;

namespace modloader
{
    int g_waitpid_result;

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

    std::uintptr_t translate_pointer(uintptr_t pid, uintptr_t source_pointer, std::string target_module_name)
    {
        return source_pointer - get_base(getpid(), target_module_name) + get_base(pid, target_module_name);
    }

    ptrace_wrapper::ptrace_wrapper(std::uintptr_t pid, std::uintptr_t p_mmap, std::uintptr_t p_munmap)
    {
        m_pid = pid;
        m_p_mmap = p_mmap;
        m_p_munmap = p_munmap;
        m_has_errored = true;
        m_shellcode_address = 0;

        // Attach to process. This will pause it so we can safely do our magic
        if (! ptrace_succeeded(ptrace(PTRACE_ATTACH, m_pid, nullptr, nullptr))) return;
        if (! wait(m_pid)) return;

        // Save the current registers so we can restore them when we're done
        if (! ptrace_succeeded(ptrace(PTRACE_GETREGS, m_pid, nullptr, &m_original_regs))) return;
        // Second call could / should be a memcpy
        if (! ptrace_succeeded(ptrace(PTRACE_GETREGS, m_pid, nullptr, &m_regs))) return;

        // Allocate some memory to place shellcode
        m_shellcode_size = 0x420;
        m_shellcode_address = execute_single_syscall(p_mmap, 0, m_shellcode_size, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_PRIVATE | MAP_ANONYMOUS, -1, 0);
        m_has_errored = false;
    }

    std::uintptr_t ptrace_wrapper::execute_single_syscall(std::uintptr_t address, std::uintptr_t arg1, std::uintptr_t arg2, std::uintptr_t arg3, std::uintptr_t arg4, std::uintptr_t arg5, std::uintptr_t arg6)
    {
        m_regs.rip = address;
        m_regs.rax = 0x1337;
        m_regs.rdi = arg1;
        m_regs.rsi = arg2;
        m_regs.rdx = arg3;
        m_regs.rcx = arg4;
        m_regs.r8 = arg5;
        m_regs.r9 = arg6;

        // Copy instruction pointer and arguments to target process
        if (! ptrace_succeeded(ptrace(PTRACE_SETREGS, m_pid, nullptr, &m_regs))) return 0;

        // Unfreeze target process for 1 syscall, we call twice because ptrace will halt both before and after executing the syscall
        if (! ptrace_succeeded(ptrace(PTRACE_SYSCALL, m_pid, nullptr, nullptr))) return 0;
        if (! wait(m_pid)) return 0;
        std::cout << "syscall enter" << std::endl;
        if (! ptrace_succeeded(ptrace(PTRACE_SYSCALL, m_pid, nullptr, nullptr))) return 0;
        std::cout << "syscall exit" << std::endl;
        if (! wait(m_pid)) return 0;

        // Retrieve the returned value
        if (! ptrace_succeeded(ptrace(PTRACE_GETREGS, m_pid, nullptr, &m_regs))) return 0;
        return m_regs.rax;
    }

    bool ptrace_wrapper::write_to(std::uintptr_t address, int size, char* in_buffer)
    {
        size_t start = 0;
        long tmp;
        while(start < size)
        {
            memcpy(&tmp, in_buffer+start, sizeof(tmp));
            if (! ptrace_succeeded(ptrace(PTRACE_POKETEXT, m_pid, (address + start), tmp))) return false;
            start+=sizeof(tmp);
        }
        return true;
    }

    void ptrace_wrapper::read_from(std::uintptr_t address, int size, char* out_buffer)
    {
        size_t start = 0;
        long tmp;
        while(start < size)
        {
            tmp = ptrace(PTRACE_PEEKTEXT, m_pid, address + start, 0);
            memcpy(out_buffer + start, &tmp, sizeof(tmp));
            start+=sizeof(tmp);
        }
    }

    std::uintptr_t ptrace_wrapper::execute_function(std::uintptr_t address, std::uintptr_t arg1)
    {

        return 0;
    }

    bool ptrace_wrapper::has_errored()
    {
        return m_has_errored;
    }

    ptrace_wrapper::~ptrace_wrapper()
    {
        if (m_shellcode_address)
        {
            execute_single_syscall(m_p_munmap, m_shellcode_address, m_shellcode_size);
        }

        if (! m_has_errored)
        {
            ptrace(PTRACE_SETREGS, m_pid, nullptr, &m_original_regs);
            ptrace(PTRACE_DETACH, m_pid, nullptr, nullptr);
        }
    }
}
