#include "refs.h"

#include <iostream>
#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;

using namespace imperium;

Refs::Refs(fs::path path)
{
    _head_path = head_path(path);
}

void Refs::update_head(std::string new_sha)
{
    auto HEAD = std::fstream(_head_path, std::ios::out);
    HEAD << new_sha;
    HEAD.close();
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