#include <Windows.h>
#include <libmod/app.hh>
#include <libmod/hooks.hh>

void test() {
    dbg("xd");
}

mod::hooks::imgui_dx11_hook_t dxhook(test);

bool mod::init()
{
    dbg("hello");

    dxhook.enable();

    uninject();
    return true;
}

void mod::exit()
{
    while (true)
    {
        if (GetAsyncKeyState(VK_ESCAPE) != 0) break;
        Sleep(100);
    }
    dbg("goodbye");
    dxhook.disable();
    Sleep(1000);
}

libmod_entrypoint()
