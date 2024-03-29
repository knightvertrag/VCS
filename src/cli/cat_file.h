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
        bool p;
        bool t;
        bool s;
    };

    inline void runCatFile(CatFileOptions const &op)
    {
        auto repo = imperium::repo_find();
        auto obj = imperium::object_read(repo, op.sha);
        if (op.t)
        {
            std::cout << obj->type << "\n";
            return;
        }
        else if (op.p)
        {
            obj->pretty_print();
            return;
        }
        else if (op.s)
        {
            std::cout << obj->data.substr(obj->data.find(" ") + 1, obj->data.find('\0') - obj->data.find(" ")) << "\n";
            return;
        }
        delete obj;
    }

    inline void setupCatFile(CLI::App &app)
    {
        auto options = std::make_shared<CatFileOptions>();
        auto cat_file = app.add_subcommand("cat-file", "Show contents of object");
        cat_file->require_option(2);
        cat_file->add_option("sha", options->sha, "SHA of the object file")->required();
        cat_file->add_flag("-p", options->p, "Pretty print contents of object based on type");
        cat_file->add_flag("-t", options->t, "Show the object type");
        cat_file->add_flag("-s", options->s, "Show the object size");
        cat_file->callback([options]()
                           { runCatFile(*options); });
    }
} // namespace Imperium
