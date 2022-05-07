#include <app.hh>
#include <ui.hh>
#include <cfg.hh>
#include <processapi.hh>
#include <inject.hh>

#include <chrono>
#include <thread>
#include <vector>
#include <map>
#include <string>
#include <cstdint>

namespace modloader::app
{
    std::thread* injector_thread = nullptr;
    bool running = false;
    injection_ctx_t ctx;

    void loop()
    {
        auto curr_dir = get_current_module_dir();
        auto mods_dir = curr_dir / "mods";

        running = true;
        while (running)
        {
            auto conf = cfg::get();
            for (auto& proc : get_processes())
            {
                // Skip processes that are either not in the ini, or explicitely disabled
                auto target = conf->targets.find(proc.exe_name);
                if (target == conf->targets.end()) continue;
                if (! target->second.enabled) continue;

                // Find mods for this process
                auto module_mods_dir = mods_dir / proc.exe_name;
                if (! std::filesystem::exists(module_mods_dir)) continue;
                for (auto& file : std::filesystem::directory_iterator(module_mods_dir))
                {
                    // Check if mod is actually an executable / injectable file
                    if (! is_injectable_file(file)) continue;
                    
                    // Check if this process has a cache, if not create it
                    if (ctx.pid_mod_time_map.find(proc.pid) == ctx.pid_mod_time_map.end())
                    {
                        ctx.pid_mod_time_map[proc.pid] = std::map<std::string, std::filesystem::file_time_type>();
                    }
                    auto mod_time_map = ctx.pid_mod_time_map[proc.pid];

                    // Check if the mod has a cached injection time, and if so, check if file_modify_time > injection_time
                    auto path_string = file.path().string();
                    if (mod_time_map.find(path_string) != mod_time_map.end())
                    {
                        if (! target->second.dev_last_updated) continue;

                        auto file_modified_time = std::filesystem::last_write_time(file.path());

                        if (file_modified_time <= mod_time_map[path_string]) continue;
                    }

                    ctx.pid_mod_time_map[proc.pid][path_string] = std::filesystem::last_write_time(file.path());

                    auto result = inject(proc.pid, file.path());
                    if (! result)
                    {
                        ui::raw_log_fmt("inject failed!");
                    }
                }
            }
            std::this_thread::sleep_for(std::chrono::milliseconds(100));
        }
    }

    bool is_running()
    {
        return running;
    }

    void stop()
    {
        running = false;
        injector_thread->join();
    }

    void init()
    {
        ui::init();
        cfg::load();
        injector_thread = new std::thread(loop);
        ui::enter_loop();
        stop();
    }


    void exit()
    {
        ui::raw_log_fmt("bye!\n");
        ui::exit();
    }
}
