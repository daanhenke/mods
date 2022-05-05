#include <libmod/app.hh>

namespace mod::impl
{
    void exit_hook()
    {
        mod::exit();
    }

    void init_hook()
    {
        if (! mod::init())
        {
            // TODO: Handle error
        }
    }
}

