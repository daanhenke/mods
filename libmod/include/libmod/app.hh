#pragma once

#include <thread>

namespace mod
{
    bool init();
    void exit();
    void uninject();

    namespace impl
    {
        void init_hook();
        void exit_hook();

        typedef struct init_exit_t
        {
            std::thread init_thread;
            init_exit_t()
            {
                init_thread = std::thread(init_hook);
                init_thread.detach();
            }

            ~init_exit_t()
            {
                exit_hook();
            }
        } init_exit_t;
    }
}

#define libmod_entrypoint() static mod::impl::init_exit_t __init_exit;
