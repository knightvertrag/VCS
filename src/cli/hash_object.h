#pragma once

#include <CLI11.hpp>
#include <memory>
#include <sstream>
#include <repository.h>
#include <imperium_object.h>
#include <fstream>
#include <filesystem>
#include <blob_object.h>

namespace fs = std::filesystem;

namespace imperium
{
    struct HashObjectOptions
    {
        fs::path path;
    };

    inline void runHashObject(HashObjectOptions &opt)
    {
        std::string sha = Blobobject::blob_from_file(opt.path);
        std::cout << sha << "\n";
    }

    inline void setupHashObject(CLI::App &app)
    {
        auto options = std::make_shared<HashObjectOptions>();
        auto hash_object = app.add_subcommand("hash-object", "Compute object ID and creates a blob from a file");
        hash_object->add_option("Path", options->path, "Path to the file")->required();
        hash_object->callback([options]()
                              { runHashObject(*options); });
    }
} // namespace imperium
