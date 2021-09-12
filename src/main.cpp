#include <iostream>
#include <filesystem>
#include <CLI11.hpp>
#include <cli/init.h>
#include <cli/cat_file.h>
#include "repository.h"
#include "imperium_object.h"

namespace fs = std::filesystem;

int main(int argc, char *argv[])
{
    imperium::Repository repo;
    CLI::App app{"A Version Control System written in C++"};
    app.require_subcommand(1);
    imperium::setupInit(app);
    imperium::setupCatFile(app);
    // auto cat_file = app.add_subcommand("cat-file", "Parse an object file");
    // std::string cat_file_sha;
    // cat_file->add_option("hash", cat_file_sha, "Hash of the object");
    // bool cat_type;
    // bool cat_content;
    // cat_file->add_flag("-t", cat_type, "Display type of object")->excludes("-p");
    // cat_file->add_flag("-p", cat_content, "Display content of object")->excludes("-t");
    // if (cat_content)
    // {
    //     auto content = imperium::object_read(repo, cat_file_sha);
    //     std::cout << content->type;
    // }

    // else if (cat_type)
    // {
    //     //display type
    // }

    CLI11_PARSE(app, argc, argv);
    return 0;
}