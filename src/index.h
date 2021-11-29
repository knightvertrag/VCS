#pragma once

#include "lockfile.h"

#include <iostream>
#include <vector>
#include <unordered_set>
#include <map>
#include <unordered_map>
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
        // store folders and map them to entries under them.
        // This will make deleting entries easier as we can directly get all the entries
        // that need to be deleted if we delete a folder
        std::unordered_map<std::string, std::unordered_set<std::string>> _parents;

    private:
        LockFile m_lock_file;

        void store_entry(const Entry &__entry);
        void remove_entry(const std::string &__path);
        void remove_children(const std::string &__path);
        void discard_conflicts(const Entry &__entry);
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
            2 byte set of flags with following format:
            assume-valid extended stage name_length
            1         1         2      12
            0         0        00   000000000000
        } -> padded with zeroes to make entry length multiple of 8
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

        // return all parent paths of _path of current entry
        std::vector<std::string> parent_dirs();

        // Serialize and put the __entry in __stream
        friend std::ostringstream &operator<<(std::ostringstream &__stream, Index::Entry &__entry);

        // Deserialize __stream and use it to fill up __entry
        friend void operator>>(std::ifstream &__stream, Index::Entry &__entry);
    };
} // namespace imperium
