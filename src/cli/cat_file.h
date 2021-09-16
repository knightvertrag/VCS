#pragma once

#include <memory>
#include <CLI11.hpp>
#include <repository.h>
#include <imperium_object.h>

namespace imperium
{
    struct CatFileOptions
    {
        std::string sha;
    };

    void runCatFile(CatFileOptions const &op)
    {
        auto repo = imperium::repo_find();
        auto obj = imperium::object_read(repo, op.sha);
        std::cout << obj->data << "\n";
        delete obj;
    }

    void setupCatFile(CLI::App &app)
    {
        auto options = std::make_shared<CatFileOptions>();
        auto cat_file = app.add_subcommand("cat-file", "Show contents of object");
        cat_file->add_option("sha", options->sha, "SHA of the object file")->required();
        cat_file->callback([options]()
                           { runCatFile(*options); });
    }
} // namespace Imperium
