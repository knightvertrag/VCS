#include "lockfile.h"

#include <iostream>
#include <filesystem>
#include <fcntl.h>
#include <unistd.h>
#include <cerrno>
#include <exception>

using namespace imperium;

class StaleLock : public std::exception
{
public:
    std::string msg;
    StaleLock(std::string lock_path) : msg("Not holding lock on file: " + lock_path) {}
    const char *what() const throw()
    {
        return msg.c_str();
    }
};

LockFile::LockFile(fs::path path)
{
    _file_path = path;
    _lock_path = fs::path(path.generic_string() + ".lock");
    _lock = 0;
}

int LockFile::hold_for_update()
{
    if (!_lock)
    {
        _lock = open(_lock_path.c_str(), O_RDWR | O_CREAT | O_EXCL, 0666);
        if (_lock < 0)
        {
            if (_lock == EEXIST)
                return false;
            if (_lock == ENOENT)
                throw ENOENT;
            else if (_lock == EACCES)
                throw EACCES;
        }
    }
    return true;
}

void LockFile::raise_on_stale_lock()
{
    if (!_lock)
    {
        throw StaleLock(_lock_path.generic_string());
    }
}

void LockFile::write_lock(const std::string &s)
{
    raise_on_stale_lock();
    int buf_size = s.size();
    char buffer[buf_size];
    for (int i = 0; i < buf_size; i++)
        buffer[i] = s[i];
    write(_lock, buffer, buf_size);
}

void LockFile::commit_lock()
{
    raise_on_stale_lock();
    close(_lock);
    fs::rename(_lock_path, _file_path);
    _lock = 0;
}
