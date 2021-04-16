#include <iostream>

#include "cli.h"
#include "repository.h"
#include "imperium_object.h"

int main(int argc, char *argv[])
{
    imperium::Repository repo;
    if (strcmp(argv[1], "init") == 0)
    {
        if (argc == 3)
            imperium::init(argv[3]);
        else
            imperium::init();
    }
    if (strcmp(argv[1], "cat-file") == 0)
    {
        repo = imperium::repo_find();
        imperium::object_read(repo, "0a868b7eab9caee8546556201c0d29f50440b0d8");
    }
    return 0;
}