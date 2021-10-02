#pragma once

#include "repository.h"
#include "imperium_object.h"

#include <map>

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

    /**
     * Parse data of format key-value pairs and a message. Used in parsing commits and tags.
     * 
     * @param data Raw string to be parsed
     * @return Parsed map
    */
    std::map<std::string, std::string> kv_parse(std::string data);

}
