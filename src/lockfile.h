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
        // Append ".lock" to path and set _lock_path and _file_path
        LockFile(fs::path path);
        /**
         * Attempt to acquire lock. Returns false if lockfile already exists,
         * which implies some other process is using it.
         *
         */
        int hold_for_update();
        /**
         * Throws exception if _lock = 0. Must be called before writing
         * or commiting to lockfile since current process may not own the lock
         */
        void raise_on_stale_lock();
        /**
         * Write contents of s to lockfile
         *
         * @param s String to be written
         */
        void write_lock(const std::string &s);
        /**
         * Close the lockfile and rename to oringinal filename
         */
        void commit_lock();
    };

} // namespace imperium
