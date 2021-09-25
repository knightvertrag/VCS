#pragma once

#include <memory>
#include <CLI11.hpp>
#include <repository.h>

namespace imperium
{
    struct InitOptions
    {
        std::string path;
    };

    inline void runInit(InitOptions const &op)
    {
        std::cout << "Initializing Imperium Repository...\n";
        imperium::repo_create(op.path);
        std::cout << "Done\n";
    }
    inline void setupInit(CLI::App &app)
    {
        auto options = std::make_shared<InitOptions>();
        options->path = "./";
        auto init = app.add_subcommand("init", "Initialize an Imperium Repository");
        init->add_option("-p, --path", options->path, "Path to repository");
        init->callback([options]()
                       { runInit(*options); });
    }

} // namespace imperium
