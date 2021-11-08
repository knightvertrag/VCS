#pragma once

#include <iostream>
#include <filesystem>

namespace fs = std::filesystem;

namespace imperium
{
    class LockFile
    {
    public:
        fs::path _file_path;
        fs::path _lock_path;
        int _lock;
        LockFile() = default;
        LockFile(fs::path path);
        int hold_for_update();
        void raise_on_stale_lock();
        void write_lock(const std::string &s);
        void commit_lock();
    };

} // namespace imperium
