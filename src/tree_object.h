#pragma once

#include "imperium_object.h"
#include "repository.h"

#include <iostream>
#include <filesystem>
#include <vector>
#include <map>
#include <ordered_map.h>
#include <variant>

namespace fs = std::filesystem;

namespace imperium
{
    class TreeLeaf
    {
    public:
        std::string mode;
        fs::path path;
        std::string sha;

        TreeLeaf() = default;
        TreeLeaf(std::string mode, fs::path path, std::string sha) : mode(mode), path(path), sha(sha){};
        TreeLeaf(TreeLeaf &&__other) = default;
        TreeLeaf &operator=(TreeLeaf &&__other) = default;
        TreeLeaf(const TreeLeaf &__other) = default;
        TreeLeaf &operator=(const TreeLeaf &__other) = default;
    };

    class Treeobject : public Impobject
    {
    private:
        std::vector<TreeLeaf> _items;
        vertrag::ordered_map<std::string, std::variant<TreeLeaf, Treeobject>> _entries;

    public:
        Treeobject() : Impobject(repo_find(), "", "tree"){};
        Treeobject(Repository repo, std::string data);
        Treeobject(std::vector<TreeLeaf> items, Repository repo, std::string data) : Impobject(repo, data, "tree"), _items(items) {}
        void pretty_print();
        std::string serialize();
        void deserialize(std::string data);

        /**
         * Recursively adds trees/blobs to the tree object it's called on
         *
         * @param parents vector of paths representing the current parents of item
         * @param item TreeLeaf to be inserted to this object
         */
        void add_entry(std::vector<fs::path> &parents, TreeLeaf &item);

        /**
         * Recursively traverse through current Treeobject while writing nested trees to filesystem
         *
         * @return sha of tree created
         */
        std::string traverse();

        /**
         * Iterate through __entries and build out the root tree object
         *
         * @param __entries vector of TreeLeaf containing all paths to build root tree with
         * @return root tree object
         */
        static Treeobject *build(std::vector<TreeLeaf> &__entries);
        /**
         * This is just a convenience method for testing commits, will be removed when INDEX is added
         */
        static std::string construct_tree();
    };
} // namespace imperium
