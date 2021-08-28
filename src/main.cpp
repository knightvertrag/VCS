#include <iostream>
#include <filesystem>
#include <CLI11.hpp>
#include "cli.h"
#include "repository.h"
#include "imperium_object.h"

namespace fs = std::filesystem;

int main(int argc, char *argv[])
{
    imperium::Repository repo;
    CLI::App app{"A Version Control System written in C++"};
    app.require_subcommand(1);
    auto init = app.add_subcommand("init", "Initialize an Imperium Repository");
    fs::path path = "./";
    init->add_option("path", path, "Path to repository");
    init->callback([&]()
                   {
                       std::cout << "Initializing Imperium Repository...\n";
                       repo = imperium::init(path);
                       std::cout << "Done\n";
                   });
    auto cat_file = app.add_subcommand("cat-file", "Parse an object file");
    std::string cat_file_sha;
    cat_file->add_option("hash", cat_file_sha, "Hash of the object");
    bool cat_type;
    bool cat_content;
    cat_file->add_flag("-t", cat_type, "Display type of object")->excludes("-p");
    cat_file->add_flag("-p", cat_content, "Display content of object")->excludes("-t");
    if (cat_content)
        imperium::object_read(repo, cat_file_sha);
    else if (cat_type)
        //display type
        CLI11_PARSE(app, argc, argv);
    return 0;
}