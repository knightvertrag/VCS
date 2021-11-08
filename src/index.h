#pragma once

#include "lockfile.h"

#include <iostream>
#include <map>
#include <string>
#include <filesystem>
#include <sys/stat.h>

namespace fs = std::filesystem;

namespace imperium
{
    class Index
    {
    public:
        static constexpr int REGULAR_MODE = 0100644;
        static constexpr int EXECUTABLE_MODE = 0100755;
        static constexpr size_t MAX_PATH_SIZE = 0xfff;

        class Entry;
        Index() = default;

        Index(fs::path __path) : m_lock_file(__path) {}

        Index(const Index &__other) = default;
        Index &operator=(const Index &__other) = default;

        Index(Index &&__other) = default;
        Index &operator=(Index &&__other) = default;

        void add(fs::path &__path, std::string &__sha);
        bool write_updates();
        void write(const std::string &data);
        void end_write();

    private:
        // map entries to their paths
        std::map<std::string, Entry> m_entries;
        LockFile m_lock_file;
    };
    /*
        Entry Format

        32-bit ctime seconds, the last time a file's metadata changed
        32-bit ctime nanosecond fractions
        32-bit mtime seconds, the last time a file's data changed
        32-bit mtime nanosecond fractions
        32-bit dev
        32-bit ino
        32-bit mode
        32-bit uid
        32-bit gid
        32-bit file size
        {
            2-byte set of flags(onr byte contains length of file pathname)
            file pathname
        } -> padded with zeroes to get multiple of 8
        20-byte hash of index contents
    */
    class Index::Entry
    {
    public:
        fs::path _path;
        std::string _sha;
        int _flags;
        struct stat _stat;

        Entry() = default;
        Entry(fs::path &__path, std::string &__sha);

        friend std::ostringstream &operator<<(std::ostringstream &__stream, Index::Entry &__entry);
    };
} // namespace imperium
