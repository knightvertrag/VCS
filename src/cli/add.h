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
        for (auto s : opt.paths)
        {
            fs::path absolute_path = fs::current_path() / fs::path(s);
            if (fs::is_regular_file(absolute_path))
            {
                auto oid = Blobobject::blob_from_file(absolute_path);
                index.add(fs::relative(absolute_path, repo.worktree), oid);
            }
            else
            {
                for (auto p = fs::recursive_directory_iterator(absolute_path);
                     p != fs::recursive_directory_iterator();
                     p++)
                {
                    if (fs::is_regular_file(p->path()))
                    {
                        auto oid = Blobobject::blob_from_file(p->path());
                        index.add(fs::relative(p->path(), repo.worktree), oid);
                    }
                    if (p->path() == repo.impDir) // imperiumignore will generalize this
                    {
                        p.disable_recursion_pending();
                    }
                }
            }
        }
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
