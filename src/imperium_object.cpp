#include "imperium_object.h"
#include "tree_object.h"
#include "blob_object.h"
#include "commit_object.h"

#include <filesystem>
#include <fstream>
#include <algorithm>
#include <map>
#include <set>
#include <boost/compute/detail/sha1.hpp>
#include <iostream>
#include <boost/iostreams/filtering_streambuf.hpp>
#include <boost/iostreams/copy.hpp>
#include <boost/iostreams/filter/zlib.hpp>

#include "repository.h"
namespace fs = std::filesystem;

imperium::Impobject *imperium::object_read(Repository repo, std::string sha)
{
    std::string subdir = sha.substr(0, 2);
    std::string objfile = sha.substr(2);
    fs::path path = repo_file(repo, {"objects", subdir, objfile});
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
    std::string obj_type = raw.substr(0, raw.find(" "));
    std::string data = raw.substr(raw.find(" ") + 1);
    if (obj_type == "blob")
    {
        Blobobject *blob = new Blobobject(repo, raw);
        return blob;
    }
    else if (obj_type == "tree")
    {
        Treeobject *tree = new Treeobject(repo, raw);
        return tree;
    }
    else if (obj_type == "commit")
    {
        Commitobject *commit = new Commitobject(repo, raw);
        return commit;
    }
    // else if(obj_type == "tag")
    return nullptr;
}

std::string imperium::object_write(Impobject &obj, bool actually_write)
{
    std::string data = obj.serialize();
    std::string result = obj.type + " " + std::to_string(obj.data.size()) + '\0' + data;
    std::string sha = boost::compute::detail::sha1(result);
    std::string folder_name = sha.substr(0, 2);
    std::string file_name = sha.substr(2);
    if (actually_write || (fs::exists(fs::path(folder_name) / fs::path(file_name))))
    {
        std::stringstream compressed;
        std::stringstream decompressed;
        decompressed << result;
        boost::iostreams::filtering_streambuf<boost::iostreams::input> out;
        out.push(boost::iostreams::zlib_compressor());
        out.push(decompressed);
        boost::iostreams::copy(out, compressed);
        fs::path path = repo_file(obj.repo, {"objects", folder_name, file_name}, actually_write);
        std::ofstream file(path, std::ios::out | std::ios::binary);
        file << compressed.str();
        file.close();
        return sha;
    }
    return sha;
}

std::map<std::string, std::string> imperium::kv_parse(std::string data)
{
    std::map<std::string, std::string> res;
    std::stringstream stream;
    stream << data;
    std::string line;
    bool msg_started = false;
    while (std::getline(stream, line, '\n'))
    {
        if (line == "" || msg_started)
        {
            res["message"] += line;
            msg_started = true;
            continue;
        }
        std::string key = line.substr(0, line.find(' '));
        std::string value = line.substr(line.find(' ') + 1);
        res[key] = value;
    }
    return res;
}

std::string imperium::kv_serialize(std::map<std::string, std::string> kv, std::vector<std::string> order)
{
    std::string res;
    for (auto &i : order)
    {
        if (i == "message")
        {
            res += '\n';
            res += kv[i];
            res += "\n\n";
            continue;
        }
        res += i;
        res += " ";
        res += kv[i];
        res += '\n';
    }
    return res;
}