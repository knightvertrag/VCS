#include "index.h"
#include "lockfile.h"

#include <iostream>
#include <map>
#include <string>
#include <filesystem>
#include <memory>
#include <sstream>
#include <boost/algorithm/hex.hpp>
#include <boost/compute/detail/sha1.hpp>

#include <sys/stat.h>

using namespace imperium;
namespace fs = std::filesystem;

// Check if the host processor is big endian
bool is_big_endian()
{
    unsigned int x = 1;
    auto c = reinterpret_cast<char *>(&x);
    if (*c)
        return false;
    return true;
}

// swap endianess of 4byte/32bit integer
int &swap_endian(int &x)
{
    int lmost = (x & 0x000000FF) >> 0;
    int lmid = (x & 0x0000FF00) >> 8;
    int rmid = (x & 0x00FF0000) >> 16;
    int rmost = (x & 0xFF000000) >> 24;
    lmost <<= 24;
    lmid <<= 16;
    rmid <<= 8;
    rmost <<= 0;
    x = lmost | lmid | rmid | rmost;
    return x;
}

void Index::add(fs::path &__path, std::string &__sha)
{
    Index::Entry entry{__path, __sha};
    m_entries[entry._path.generic_string()] = entry;
}

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

Index::Entry::Entry(fs::path &__path, std::string &__sha) : _path(__path), _sha(__sha)
{
    stat(__path.c_str(), &_stat);
    if (_stat.st_mode & S_IEXEC)
        _stat.st_mode = Index::EXECUTABLE_MODE;
    if (_stat.st_mode & S_IFREG)
        _stat.st_mode = Index::REGULAR_MODE;
    _flags.assume_valid = 0;
    _flags.stage = 0;
    _flags.file_name_length = std::min(MAX_PATH_SIZE, __path.generic_string().size());
}

void write_int_item(int __prop, std::ostringstream &__stream)
{
    if (!is_big_endian())
        swap_endian(__prop);
    auto ptr = reinterpret_cast<char *>(&__prop);
    __stream.write(ptr, sizeof(int));
}

void write_flags(Index::Entry::Flags &__flags, std::ostringstream &__stream)
{
    if (__flags.file_name_length > 0xfff)
        __flags.file_name_length = 0xfff;
    int file_length = __flags.file_name_length;
    int assume_valid = __flags.assume_valid;
    int extended = __flags.extended;
    int stage = __flags.stage;
    int flags;
    stage <<= 12;
    extended <<= 14;
    assume_valid <<= 15;
    flags = file_length | assume_valid | extended | stage;
    if (!is_big_endian())
        swap_endian(flags);
    auto bytes = reinterpret_cast<char *>(&flags);
    __stream.write((bytes + 2), 2);
}

void write_file_name(const std::string &__file_name, std::ostringstream &__stream)
{
    char padding[8];
    int padding_num = 8 - ((__stream.str().size() + __file_name.size() - 12) % 8);

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

namespace imperium
{
    std::ostringstream &operator<<(std::ostringstream &__stream, Index::Entry &__entry)
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
        write_entry_sha(__entry._sha, __stream);
        write_flags(__entry._flags, __stream);
        write_file_name(__entry._path.generic_string(), __stream);
        return __stream;
    }
}

bool Index::write_updates()
{
    if (!m_lock_file.hold_for_update())
        return false;
    std::ostringstream header;
    header.write("DIRC", 4);
    int ver = 2;
    write_int_item(ver, header);
    auto num_entries = m_entries.size();
    write_int_item(num_entries, header);
    for (auto &[key, entry] : m_entries)
    {
        header << entry;
    }
    write_index_sha(header);
    this->write(header.str());
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