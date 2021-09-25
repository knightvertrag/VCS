#pragma once

#include <memory>
#include <CLI11.hpp>
#include <repository.h>
#include <imperium_object.h>
#include <tree_object.h>
#include <filesystem>

namespace fs = std::filesystem;

namespace imperium
{
    struct WriteTreeOptions
    {
        std::vector<fs::path> paths;
    };

    inline void runWriteTree(WriteTreeOptions const &op)
    {
        std::shared_ptr<Treeobject> tree = Treeobject::construct_tree(op.paths);
        std::cout << object_write(*tree) << "\n";
    }

    inline void setupWriteTree(CLI::App &app)
    {
        auto options = std::make_shared<WriteTreeOptions>();
        auto write_tree = app.add_subcommand("write-tree", "Write a tree from given paths");
        write_tree->require_option(1);
        write_tree->add_option("files", options->paths, "List of files to write to tree");
        write_tree->callback([options]()
                             { runWriteTree(*options); });
    }

} // namespace imperium
