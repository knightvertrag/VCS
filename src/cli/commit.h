#pragma once

#include <commit_object.h>
#include <memory>
#include <CLI11.hpp>
#include <repository.h>

namespace imperium
{
    struct CommitOptions
    {
        std::string message;
    };

    inline void runCommit(CommitOptions &op)
    {
        std::shared_ptr<Commitobject> obj = std::make_shared<Commitobject>(op.message);
        std::cout << object_write(*obj) << "\n";
    }

    inline void setupCommit(CLI::App &app)
    {
        std::shared_ptr<CommitOptions> options = std::make_shared<CommitOptions>();
        auto commit = app.add_subcommand("commit", "Make a commit");
        commit->add_option("-m", options->message, "Commit message");
        commit->callback([options]()
                         { runCommit(*options); });
    }

} // namespace imperium
