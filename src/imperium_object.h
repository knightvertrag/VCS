#ifndef IMPERIUM_IMPERIUM_OBJECT_H_
#define IMPERIUM_IMPERIUM_OBJECT_H_

#include "repository.h"

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

namespace imperium
{
    class Impobject
    {
    public:
        imperium::Repository repo;
        std::string type;
        std::string data;
        //template <typename T>
        Impobject(imperium::Repository repo, std::string data, std::string type) : repo(repo), type(type), data(data){};
        // void serialize();
        // void deserialize();
    };

    /**
     * Read the Impobject_id from Imperium Repo
     * 
     * @param repo Repository whose Impobjects need to be read
     * @param sha SHA-1 to extract path
     * @return Imperium Impobject required
    */
    Impobject object_read(Repository repo, std::string sha);
    template <typename T>
    std::string object_write(T &obj, bool actually_write = true)
    {
        std::string data = obj.serialize();
        std::string result = obj.type + " " + std::to_string(obj.data.size() + 1) + "\x00" + data;
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
            std::ofstream file(path, std::ios::out | std::ios::binary);
            file << compressed.str();
            return sha;
        }
        return "big fail";
    }

    class Blobobject : public Impobject
    {
    public:
        Blobobject(imperium::Repository repo, std::string data);
        std::string serialize();
        void deserialize(std::string data);
    };

    class Treeobject : public Impobject
    {
    public:
        Treeobject(Repository repo, std::string data);
        void serialize(Treeobject treeobj);
    };

    class Commitobject : public Impobject
    {
    public:
        Commitobject(Repository repo, std::string data);
        void serialize(Commitobject commitobj);
    };

    class Tagobject : public Impobject
    {
    public:
        Tagobject(Repository repo, std::string data);
        void serialize(Tagobject tagobj);
    };
}

#endif // IMPERIUM_OBJECT_H_