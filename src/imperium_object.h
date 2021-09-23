#pragma once

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
    /**
     * Abstract class to provide an interface for the concrete object types
    */
    class Impobject
    {
    public:
        imperium::Repository repo;
        std::string type;
        std::string data;
        Impobject(imperium::Repository repo, std::string data, std::string type) : repo(repo), type(type), data(data){};

        virtual std::string serialize() = 0;
        virtual void deserialize(std::string data) = 0;
        virtual void pretty_print() = 0;
    };

    /**
     * Read the object file whose sha is provided.
     * 
     * @param repo Repository whose Impobjects need to be read
     * @param sha SHA-1 to extract path
     * @return Pointer of Impobject required
    */
    Impobject *object_read(Repository repo, std::string sha);

    /**
     * Serialize provided object, compress and write to object database. Returns the SHA-1 checksum.
     * 
     * @param obj Reference to the object being written
     * @param actually_write Default = true. If false return sha and exit
     * @return SHA-1 Checksum of the contents 
    */
    std::string object_write(Impobject &obj, bool actually_write = true);

    class Blobobject : public Impobject
    {
    public:
        Blobobject(imperium::Repository repo, std::string data);
        void pretty_print();
        std::string serialize();
        void deserialize(std::string data);
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
