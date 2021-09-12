#include "imperium_object.h"

#include <filesystem>
#include <fstream>
#include <algorithm>
#include <iterator>
#include <zlib.h>
#include <boost/compute/detail/sha1.hpp>
#include <iostream>
#include <boost/iostreams/filtering_streambuf.hpp>
#include <boost/iostreams/copy.hpp>
#include <boost/iostreams/filter/zlib.hpp>

#include "repository.h"
namespace fs = std::filesystem;

imperium::Impobject imperium::object_read(Repository repo, std::string sha)
{
    std::string subdir = sha.substr(0, 2);
    std::string objfile = sha.substr(2);
    fs::path path = repo_file(repo, {"objects", subdir, objfile});
    std::cout << path << "\n";
    std::ifstream file(path, std::ios::binary | std::ios::in);
    std::stringstream compressed;
    std::stringstream decompressed;
    std::copy(std::istreambuf_iterator<char>(file),
              std::istreambuf_iterator<char>(),
              std::ostreambuf_iterator<char>(compressed));
    boost::iostreams::filtering_streambuf<boost::iostreams::input> in;
    in.push(boost::iostreams::zlib_decompressor());
    in.push(compressed);
    boost::iostreams::copy(in, decompressed);
    std::string raw = decompressed.str();
    // std::cout << raw;
    std::string obj_type = raw.substr(0, raw.find(" "));
    std::string data = raw.substr(raw.find(" "));
    if (obj_type == "blob")
    {
        imperium::Blobobject blob = Blobobject(repo, raw, obj_type);
        return blob;
    }
    // else if(obj_type == "tree")
    //     return Treeobject(repo, raw);
    // else if(obj_type == "commit")
    //     return Commitobject(repo, raw);
    // else if(obj_type == "tag")
    //     return Tagobject(repo, raw);
}

template <typename T>
std::string imperium::object_write(T obj, bool actually_write)
{
    std::string data = obj.serialize();
    std::string result = obj.fmt + " " + std::to_string(obj.data.size() + 1) + "\x00" + data;
    std::string sha = boost::compute::detail::sha1(result);
    std::string folder_name = sha.substr(0, 2);
    std::string file_name = sha.substr(2);
    if (actually_write)
    {
        std::stringstream compressed;
        std::stringstream decompressed;
        decompressed << result;
        boost::iostreams::filtering_streambuf<boost::iostreams::input> out;
        out.push(boost::iostreams::zlib_compressor());
        out.push(decompressed);
        boost::iostreams::copy(out, compressed);
        fs::path path = repo_file(obj.repo, {"objects", folder_name, file_name}, actually_write);
        std::ofstream file("path", std::ios::out | std::ios::binary);
        file << compressed.str();
        return sha;
    }
    return "big fail";
}

imperium::Blobobject::Blobobject(imperium::Repository repo, std::string data, std::string type) : imperium::Impobject::Impobject(repo, data, type)
{
    deserialize(data);
}

std::string imperium::Blobobject::serialize()
{
    return this->data;
}

void imperium::Blobobject::deserialize(std::string data)
{
    this->data = data;
}
