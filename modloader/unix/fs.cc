#include <unix/fs.hh>
#include <cstdlib>
#include <sstream>
#include <string>
#include <vector>

using namespace std::filesystem;

namespace modloader
{
    const std::vector<path> library_search_path = { { "/lib" }, { "/usr/lib" } };
    bool find_shared_object(std::string partial_name, path* result)
    {
        for (auto& entry : library_search_path)
        {
            // Construct full path
            auto potential_path = entry / partial_name;

            // Awesome, now find the highest number so we can find libc.so.6 for example
            if (exists(potential_path))
            {
                auto highest_num = 0;
                for (auto& dir_entry : directory_iterator(potential_path.parent_path()))
                {
                    auto potential_target = dir_entry.path();

                    // Found something that kinda matches
                    if (potential_target.stem() == potential_path.filename())
                    {
                        // Convert extension to number, 0 being invalid
                        auto curr_num = std::stoul(potential_target.extension().string().substr(1));
                        if (curr_num == 0) continue;

                        // Set highest_num to non 0
                        if (curr_num > highest_num)
                        {
                            highest_num = curr_num;
                        }
                    }
                }

                if (highest_num != 0) *result = path(potential_path.string() + "." + std::to_string(highest_num));
                else *result = potential_path;

                return true;
            }
        }

        return false;
    }
}
