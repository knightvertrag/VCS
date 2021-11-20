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

        Index(fs::path __lock_path) : m_lock_file(__lock_path) {}

        Index(const Index &__other) = default;
        Index &operator=(const Index &__other) = default;

        Index(Index &&__other) = default;
        Index &operator=(Index &&__other) = default;

        /*****************Serializers*****************/

        // Add an entry to _entries
        void add(const fs::path &__path, std::string &__sha);
        // Serialize entries by writing to a stingstream
        bool write_updates();
        // Write stringstream contents to lockfile
        void write(const std::string &data);
        // commit changes to lockfile
        void end_write();

        /*****************deserializers****************/
        void load_for_update();
        // read the index file
        void read_index();

        // map entries to their paths
        std::map<std::string, Entry> _entries;

    private:
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
        // path relative to repo.worktree
        fs::path _path;
        std::string _sha;
        struct Flags
        {
            int file_name_length;
            int assume_valid;
            const int extended = 0; // must be 0 for version 2, which we are using
            int stage;
        } _flags;
        struct stat _stat;

        Entry() = default;
        Entry(const Entry &__other);
        Entry &operator=(const Entry &__other);
        Entry(const fs::path &__path, std::string &__sha);

        // Serialize and put the __entry in __stream
        friend std::ostringstream &operator<<(std::ostringstream &__stream, Index::Entry &__entry);

        // Deserialize __stream and use it to fill up __entry
        friend void operator>>(std::ifstream &__stream, Index::Entry &__entry);
    };
} // namespace imperium
