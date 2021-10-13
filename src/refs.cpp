#include "refs.h"
#include "lockfile.h"

#include <iostream>
#include <fstream>
#include <filesystem>
#include <exception>

namespace fs = std::filesystem;

using namespace imperium;

class LockDenied : public std::exception
{
public:
    std::string msg;
    LockDenied(std::string head_path) : msg("Could not acquire lock on file: " + head_path) {}
    const char *what() const throw()
    {
        return msg.c_str();
    }
};

Refs::Refs(fs::path path)
{
    _head_path = head_path(path);
}

void Refs::update_head(std::string new_sha)
{
    // auto HEAD = std::fstream(_head_path, std::ios::out);
    // HEAD << new_sha;
    // HEAD.close();
    auto lockfile = new LockFile(_head_path);
    if (!lockfile->hold_for_update())
    {
        throw LockDenied(_head_path);
    }
    lockfile->write_lock(new_sha);
    lockfile->write_lock("\n");
    lockfile->commit_lock();
    delete lockfile;
}

std::string Refs::read_head()
{
    std::string res;
    if (fs::exists(_head_path))
    {
        auto HEAD = std::fstream(_head_path, std::ios::in);
        HEAD >> res;
        HEAD.close();
        return res;
    }
    return "";
}

fs::path Refs::head_path(fs::path path)
{
    return path / "HEAD";
}