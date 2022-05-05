#include <libmod/app.hh>
#include <Windows.h>

extern "C" IMAGE_DOS_HEADER __ImageBase;
const auto g_instance_handle = reinterpret_cast<HINSTANCE>(&__ImageBase);

void uninject_thread()
{
    FreeLibraryAndExitThread(g_instance_handle, 0);
}

namespace mod
{
    void uninject()
    {
        CreateThread(nullptr, 0, reinterpret_cast<LPTHREAD_START_ROUTINE>(uninject_thread), nullptr, 0, nullptr);
    }
}
