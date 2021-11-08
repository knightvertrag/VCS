#include "index.h"
#include "lockfile.h"

#include <iostream>
#include <map>
#include <string>
#include <filesystem>
#include <memory>
#include <sstream>
#include <boost/algorithm/hex.hpp>

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

Index::Entry::Entry(fs::path &__path, std::string &__sha) : _path(__path), _sha(__sha)
{
    stat(__path.c_str(), &_stat);
    if (_stat.st_mode & S_IEXEC)
        _stat.st_mode = Index::EXECUTABLE_MODE;
    if (_stat.st_mode & S_IFREG)
        _stat.st_mode = Index::REGULAR_MODE;
    _flags = std::min(MAX_PATH_SIZE, __path.generic_string().size());
}

void write_int_item(int __prop, std::ostringstream &__stream)
{
    if (!is_big_endian())
        swap_endian(__prop);
    auto ptr = reinterpret_cast<char *>(&__prop);
    __stream.write(ptr, sizeof(int));
}

namespace imperium
{
    // @Todo: write flags, entry file pathname, index hash
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
        auto oid = boost::algorithm::unhex(__entry._sha);
        __stream.write(oid.c_str(), oid.size());
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