#include <CLI11.hpp>
#include <cli/init.h>
#include <cli/cat_file.h>
#include <cli/hash_object.h>
#include <cli/write_tree.h>
#include <cli/commit.h>
#include <cli/add.h>

int main(int argc, char *argv[])
{
    CLI::App app{"A Version Control System written in C++"};
    app.require_subcommand(1);

    imperium::setupInit(app);
    imperium::setupCatFile(app);
    imperium::setupHashObject(app);
    imperium::setupWriteTree(app);
    imperium::setupCommit(app);
    imperium::setupAdd(app);

    CLI11_PARSE(app, argc, argv);
    return 0;
}