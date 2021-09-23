#pragma once

#include "imperium_object.h"
#include "repository.h"

namespace imperium
{
    class TreeLeaf
    {
    public:
        std::string mode;
        fs::path path;
        std::string sha;

        TreeLeaf(std::string mode, fs::path path, std::string sha) : mode(mode), path(path), sha(sha){};
    };

    class Treeobject : public Impobject
    {
    private:
        std::vector<TreeLeaf> items;

    public:
        Treeobject(Repository repo, std::string data);
        void pretty_print();
        std::string serialize();
        void deserialize(std::string data);
    };
} // namespace imperium
