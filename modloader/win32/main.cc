#include <Windows.h>
#include <ui.hh>
#include <app.hh>

int WINAPI WinMain(HINSTANCE instance, HINSTANCE prev_instance, PSTR cmdline, int cmd_show)
{
    using namespace modloader;

    app::init();
    app::exit();

    return 0;
}
