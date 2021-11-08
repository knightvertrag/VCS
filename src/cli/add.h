#pragma once

#include <CLI11.hpp>
#include <memory>
#include <vector>
#include <string>
#include <repository.h>
#include <index.h>
#include <imperium_object.h>
#include <blob_object.h>

namespace imperium
{
    struct AddOptions
    {
        std::vector<std::string> paths;
    };

    void runAdd(AddOptions &opt)
    {
        Repository repo = repo_find();
        imperium::Index index(repo.impDir / "index");
        auto path = fs::path(opt.paths[0]);
        auto oid = Blobobject::blob_from_file(repo.worktree / path);
        index.add(path, oid);
        index.write_updates();
        index.end_write();
    }

    void setupAdd(CLI::App &app)
    {
        auto options = std::make_shared<AddOptions>();
        auto add = app.add_subcommand("add", "Add file contents to the index");
        add->add_option("files", options->paths, "List of files to be added");
        add->callback([options]()
                      { runAdd(*options); });
    }
} // namespace imperium
