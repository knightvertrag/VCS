#pragma once

#include "repository.h"
#include "imperium_object.h"

namespace imperium
{
    class Blobobject : public Impobject
    {
    public:
        Blobobject(imperium::Repository repo, std::string data);
        void pretty_print();

        /**
         * Create a blob from file at path
         * @param path Path to file to be blobed
         * @return sha of the file created
        */
        static std::string blob_from_file(fs::path path);
        std::string serialize();
        void deserialize(std::string data);
    };
} // namespace imperium
