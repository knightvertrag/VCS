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
        virtual std::string serialize() = 0;
        virtual void deserialize(std::string data) = 0;
    };

    /**
     * Read the Impobject_id from Imperium Repo
     * 
     * @param repo Repository whose Impobjects need to be read
     * @param sha SHA-1 to extract path
     * @return Imperium Impobject required
    */
    Impobject *object_read(Repository repo, std::string sha);
    //template <typename T>
    std::string object_write(Impobject &obj, bool actually_write = true);

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
        std::string serialize();
    };

    class Commitobject : public Impobject
    {
    public:
        Commitobject(Repository repo, std::string data);
        std::string serialize();
    };

    class Tagobject : public Impobject
    {
    public:
        Tagobject(Repository repo, std::string data);
        std::string serialize();
    };
}

#endif // IMPERIUM_OBJECT_H_