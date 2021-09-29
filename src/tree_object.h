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
        Treeobject(std::vector<TreeLeaf> items, Repository repo, std::string data) : Impobject(repo, data, "tree"), items(items) {}
        void pretty_print();
        std::string serialize();
        void deserialize(std::string data);
        /**
         * Paths taken from the cli to construct tree object
         * 
         * @param paths vector of paths - files and directories
         * @return shared pointer of constructed tree object
        */
        static std::shared_ptr<Treeobject> construct_tree(std::vector<fs::path> paths);
    };
} // namespace imperium
