#include <inject.hh>
#include <unix/processapi_ext.hh>
#include <dlfcn.h>

namespace modloader
{
    bool inject(std::uintptr_t pid, std::filesystem::path dll_path)
    {
        // auto base = get_base(pid);
        auto handle = dlopen("libc.so.6", RTLD_LOCAL | RTLD_LAZY);
        auto local = dlsym(handle, "__libc_dlopen_mode");
        // auto remote = translate_pointer(pid, local);

        return false;
    }
}
