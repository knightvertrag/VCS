#pragma once

#include <CLI11.hpp>
#include <memory>
#include <sstream>
#include <repository.h>
#include <imperium_object.h>
#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;

namespace imperium
{
    struct HashObjectOptions
    {
        fs::path path;
        //Impobject object;
        //bool actually_write;
    };

    void runHashObject(HashObjectOptions &opt)
    {
        //We are only hashing blob files for now
        //TODO: Create a system to hash every type of object
        std::ifstream file_to_hash(opt.path, std::ios::in);
        std::stringstream blob_buffer;
        blob_buffer << file_to_hash.rdbuf();
        std::shared_ptr<Blobobject> blob = std::make_shared<Blobobject>(repo_find(), blob_buffer.str());
        std::cout << object_write(*blob, true) << "\n";
        file_to_hash.close();
    }

    void setupHashObject(CLI::App &app)
    {
        auto options = std::make_shared<HashObjectOptions>();
        auto hash_object = app.add_subcommand("hash-object", "Hash a file");
        hash_object->add_option("Path", options->path, "Path to the file")->required();
        hash_object->callback([options]()
                              { runHashObject(*options); });
    }
} // namespace imperium
