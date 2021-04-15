#include "imperium_object.h"

#include <filesystem>
#include <fstream>
#include <algorithm>
#include <iterator>
#include <zlib.h>
#include <boost/beast/core/detail/sha1.hpp>
#include <iostream>
#include <boost/iostreams/filtering_streambuf.hpp>
#include <boost/iostreams/copy.hpp>
#include <boost/iostreams/filter/zlib.hpp>

#include "repository.h"


imperium::Impobject::Impobject(imperium::Repository repo, std::string data)
{
    Impobject::repo = repo;
}

imperium::Impobject imperium::object_read(Repository repo, std::string sha)
{
    auto subdir = sha.substr(0, 2);
    auto objfile = sha.substr(2);
    fs::path path = repo_file(repo, {"objects", subdir, objfile});
    std::ifstream file(path, std::ios::in | std::ios::binary);
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
    std::string obj_type = raw.substr(0, raw.find(" "));
    std::string data = raw.substr(raw.find(" "));
    if(obj_type == "blob")
        return Blobobject(repo, raw);
    // else if(obj_type == "tree")
    //     return Treeobject(repo, raw);
    // else if(obj_type == "commit")
    //     return Commitobject(repo, raw);
    // else if(obj_type == "tag")
    //     return Tagobject(repo, raw);
}

imperium::Blobobject::Blobobject(imperium::Repository repo, std::string data)
{
    deserialize(data);
}

std::string imperium::Blobobject::serialize()
{
    return blobdata;
}

void imperium::Blobobject::deserialize(std::string data)
{
    blobdata = data;
}