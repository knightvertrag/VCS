#include "index.h"
#include "lockfile.h"
#include "repository.h"

#include <iostream>
#include <fstream>
#include <map>
#include <string>
#include <filesystem>
#include <memory>
#include <sstream>
#include <cstdint>
#include <boost/algorithm/hex.hpp>
#include <boost/compute/detail/sha1.hpp>

#include <arpa/inet.h>
#include <sys/stat.h>

using namespace imperium;
namespace fs = std::filesystem;

// Check if the host processor is big endian
// bool is_big_endian()
// {
//     unsigned int x = 1;
//     auto c = reinterpret_cast<char *>(&x);
//     if (*c)
//         return false;
//     return true;
// }

// swap endianess of 4byte/32bit integer
// void swap_endian(unsigned int &x)
// {
//     int lmost = (x & 0x000000FF) >> 0;
//     int lmid = (x & 0x0000FF00) >> 8;
//     int rmid = (x & 0x00FF0000) >> 16;
//     int rmost = (x & 0xFF000000) >> 24;
//     lmost <<= 24;
//     lmid <<= 16;
//     rmid <<= 8;
//     rmost <<= 0;
//     x = lmost | lmid | rmid | rmost;
// }

Index::Entry::Entry(const Entry &__other)
{
    _path = __other._path;
    _sha = __other._sha;
    _stat = __other._stat;
    _flags.assume_valid = __other._flags.assume_valid;
    _flags.file_name_length = __other._flags.file_name_length;
    _flags.stage = __other._flags.stage;
}

Index::Entry &Index::Entry::operator=(const Entry &__other)
{
    _path = __other._path;
    _sha = __other._sha;
    _stat = __other._stat;
    _flags.assume_valid = __other._flags.assume_valid;
    _flags.file_name_length = __other._flags.file_name_length;
    _flags.stage = __other._flags.stage;
    return *this;
}

Index::Entry::Entry(const fs::path &__path, std::string &__sha) : _path(__path), _sha(__sha)
{
    Repository repo = repo_find();
    fs::path absolute_path = repo.worktree / __path;
    stat(absolute_path.c_str(), &_stat);
    if (_stat.st_mode & S_IEXEC)
        _stat.st_mode = Index::EXECUTABLE_MODE;
    if (_stat.st_mode & S_IFREG)
        _stat.st_mode = Index::REGULAR_MODE;
    _flags.assume_valid = 0;
    _flags.stage = 0;
    _flags.file_name_length = std::min(MAX_PATH_SIZE, __path.generic_string().size());
}

void Index::add(const fs::path &__path, std::string &__sha)
{
    Index::Entry entry{__path, __sha};
    _entries[entry._path.generic_string()] = entry;
}

/************************************************Serializing************************************************/

void write_int_item(unsigned int __prop, std::ostringstream &__stream)
{
    __prop = htonl(__prop);
    auto ptr = reinterpret_cast<char *>(&__prop);
    __stream.write(ptr, sizeof(int));
}

void write_flags(Index::Entry::Flags &__flags, std::ostringstream &__stream)
{
    unsigned int file_name_length = __flags.file_name_length;
    unsigned int assume_valid = __flags.assume_valid;
    unsigned int extended = __flags.extended;
    unsigned int stage = __flags.stage;
    unsigned int flags;
    stage <<= 12;
    extended <<= 14;
    assume_valid <<= 15;
    flags = file_name_length | assume_valid | extended | stage;

    flags = htonl(flags);
    auto bytes = reinterpret_cast<char *>(&flags);
    __stream.write((bytes + 2), 2); // 16 bit required so start from third byte
}

void write_file_name(const std::string &__file_name, std::ostringstream &__stream)
{
    char padding[8];
    int padding_num = 8 - ((__stream.str().size() + __file_name.size() - 12) % 8);

    // The whole entry needs to be padded to make the length the nearest
    // multiple of 8
    for (size_t i = 0; i < padding_num; i++)
    {
        padding[i] = 0;
    }

    auto name_ptr = __file_name.c_str();
    __stream.write(name_ptr, __file_name.size());
    __stream.write(padding, padding_num);
}

void write_entry_sha(const std::string &__sha, std::ostringstream &__stream)
{
    auto oid = boost::algorithm::unhex(__sha);
    __stream.write(oid.c_str(), 20);
}

void write_index_sha(std::ostringstream &__stream)
{
    std::string index_sha = boost::compute::detail::sha1(__stream.str());
    auto index_sha_bytes = boost::algorithm::unhex(index_sha);
    __stream.write(index_sha_bytes.c_str(), 20);
}

void write_header(int __n_entries, std::ostringstream &__stream)
{
    __stream.write("DIRC", 4);
    int ver = 2;
    write_int_item(ver, __stream);
    write_int_item(__n_entries, __stream);
}

void write_stat_bytes(Index::Entry &__entry, std::ostringstream &__stream)
{
    write_int_item(__entry._stat.st_ctim.tv_sec, __stream);
    write_int_item(__entry._stat.st_ctim.tv_nsec, __stream);
    write_int_item(__entry._stat.st_mtim.tv_sec, __stream);
    write_int_item(__entry._stat.st_mtim.tv_nsec, __stream);
    write_int_item(__entry._stat.st_dev, __stream);
    write_int_item(__entry._stat.st_ino, __stream);
    write_int_item(__entry._stat.st_mode, __stream);
    write_int_item(__entry._stat.st_uid, __stream);
    write_int_item(__entry._stat.st_gid, __stream);
    write_int_item(__entry._stat.st_size, __stream);
}

namespace imperium
{
    std::ostringstream &operator<<(std::ostringstream &__stream, Index::Entry &__entry)
    {
        write_stat_bytes(__entry, __stream);
        write_entry_sha(__entry._sha, __stream);
        write_flags(__entry._flags, __stream);
        write_file_name(__entry._path.generic_string(), __stream);
        return __stream;
    }
}

bool Index::write_updates()
{
    std::ostringstream update;

    write_header(_entries.size(), update);
    for (auto &[key, entry] : _entries)
    {
        update << entry;
    }
    write_index_sha(update);

    this->write(update.str());

    return true;
}

void Index::write(const std::string &data)
{
    m_lock_file.write_lock(data);
}

void Index::end_write()
{
    m_lock_file.commit_lock();
}

/**************************************************deserializing******************************************/

int read_header(std::ifstream &__stream)
{
    int num_entries = 0;
    __stream.seekg(8, std::ios::beg);
    char buf[5];
    __stream.read(buf, 4);
    auto x = reinterpret_cast<int *>(buf);
    return ntohl(*x);
}

// read all stat items
void read_stat(std::ifstream &__stream, Index::Entry &__entry)
{
    char buffer[5];
    std::vector<uint32_t> stat;
    for (int i = 0; i < 10; i++)
    {
        __stream.read(buffer, 4);
        auto x = reinterpret_cast<unsigned int *>(buffer);
        auto item = ntohl(*x);
        stat.push_back(item);
    }
    __entry._stat.st_ctim.tv_sec = stat[0];
    __entry._stat.st_ctim.tv_nsec = stat[1];
    __entry._stat.st_mtim.tv_sec = stat[2];
    __entry._stat.st_mtim.tv_nsec = stat[3];
    __entry._stat.st_dev = stat[4];
    __entry._stat.st_ino = stat[5];
    __entry._stat.st_mode = stat[6];
    __entry._stat.st_uid = stat[7];
    __entry._stat.st_gid = stat[8];
    __entry._stat.st_size = stat[9];
}

// read sha
void read_entry_sha(std::ifstream &__stream, Index::Entry &__entry)
{
    char buffer[20];
    __stream.read(buffer, 20);
    std::string bytes;
    bytes.reserve(20);
    for (int i = 0; i < 20; i++)
    {
        bytes.push_back(buffer[i]);
    }
    auto sha = boost::algorithm::hex_lower(bytes);
    __entry._sha = sha;
}

// read flags
void read_flags(std::ifstream &__stream, Index::Entry &__entry)
{
    // assume-valid extended stage name_length
    //       1         1         2      12
    //       0         0        00   000000000000
    char buffer[4];
    __stream.read(buffer, 2);
    auto x = reinterpret_cast<unsigned int *>(buffer);
    unsigned int flags = ntohl(*x);
    flags >>= 16;
    __entry._flags.assume_valid = (flags >> 15) & ((1 << 1) - 1);
    // __entry._flags.extended = (flags >> 14) & ((1 << 1) - 1);
    __entry._flags.stage = (flags >> 12) & ((1 << 2) - 1);
    __entry._flags.file_name_length = flags & ((1 << 12) - 1);
}

// read file name for entry
void read_entry_file_name(std::ifstream &__stream, Index::Entry &__entry)
{
    char buffer[8];
    std::string path;
    if (__entry._flags.file_name_length < 0xFFF)
        path.reserve(__entry._flags.file_name_length);

    // read 2 bytes first since file name length will be min 2 bytes(1 letter and 1 null byte)
    // which adds up to 64 bytes read till now(excluding header).
    // Since entry will always be a multiple of 8, we can start reading
    // 8 bytes at a time till null is encountered

    __stream.read(buffer, 2);
    path.push_back(buffer[0]);

    // end of entry if true
    if (buffer[1] == '\0')
    {
        __entry._path = path;
        return;
    }
    // read 8 bytes at a time. Encountering null signals end of entry
    else
    {
        path.push_back(buffer[1]);
        while (true)
        {
            __stream.read(buffer, 8);
            for (int i = 0; i < 8; i++)
            {
                if (buffer[i] != '\0')
                    path.push_back(buffer[i]);
                else
                {
                    __entry._path = fs::path(path);
                    return;
                }
            }
        }
    }
}

namespace imperium
{
    void operator>>(std::ifstream &__stream, Index::Entry &__entry)
    {
        read_stat(__stream, __entry);
        read_entry_sha(__stream, __entry);
        read_flags(__stream, __entry);
        read_entry_file_name(__stream, __entry);
    }
}

void Index::load_for_update()
{
    if (m_lock_file.hold_for_update())
    {
        read_index();
    }
}

void Index::read_index()
{
    Repository repo = repo_find();
    fs::path index_path = repo.impDir / "index";
    if (fs::exists(index_path))
    {
        std::ifstream index_file(index_path, std::ios::in | std::ios::binary);
        int entries = read_header(index_file);
        for (int i = 0; i < entries; i++)
        {
            Index::Entry e;
            index_file >> e;
            _entries[e._path] = e;
        }
    }
}