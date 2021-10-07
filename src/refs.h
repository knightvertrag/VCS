#pragma once

#include <iostream>
#include <filesystem>

namespace fs = std::filesystem;

namespace imperium
{
    class Refs
    {
    public:
        fs::path _head_path;

        Refs(fs::path path);
        fs::path head_path(fs::path path);
        void update_head(std::string new_sha);
        std::string read_head();
    };

} // namespace imperium
