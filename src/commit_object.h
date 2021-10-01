#pragma once

#include "imperium_object.h"
#include <map>

namespace imperium
{
    class Commitobject : public Impobject
    {
    public:
        std::string tree;
        std::string parent;
        std::string author;
        std::string committer;
        std::string message;

        Commitobject(Repository repo, std::string data);
        Commitobject(std::string message);
        std::string serialize();
        void deserialize(std::string data);
        void pretty_print();
    };
} // namespace imperium
