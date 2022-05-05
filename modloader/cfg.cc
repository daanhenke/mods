#include <cfg.hh>
#include <ini.hh>
#include <processapi.hh>
#include <ui.hh>

#include <filesystem>
#include <fstream>

namespace modloader::cfg
{
    config_t conf;

    void load()
    {
        auto mod_dir = get_current_module_dir();
        auto config_path = mod_dir / "config.ini";

        if (! std::filesystem::exists(config_path))
        {
            std::ofstream output(config_path);
            output.close();
        }

        auto file = mINI::INIFile(config_path.string());
        auto ini = mINI::INIStructure();
        file.read(ini);

        conf = config_t();
        for (auto& it : ini)
        {
            target_t current_target;
            current_target.enabled = it.second.has("enabled") ? (it.second.get("enabled") == "true") : true;
            current_target.dev_mode = it.second.has("dev_mode") ? (it.second.get("dev_mode") == "true") : true;
            current_target.dev_last_updated = std::chrono::milliseconds(std::time(nullptr)).count();

            conf.targets[it.first] = current_target;
        }
    }

    config_t* get()
    {
        return &conf;
    }
}
