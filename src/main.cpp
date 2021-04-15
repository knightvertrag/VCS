#include <iostream>
#include "repository.h"
#include "imperium_object.h"

int main(int argc, char *argv[])
{
    imperium::Repository repo;
    if (strcmp(argv[1], "init") == 0)
    {
        if (argc == 3)
        {
            try
            {
                repo = imperium::repo_create(argv[3]);
            }
            catch (const std::exception &e)
            {
                std::cerr << e.what() << '\n';
            }
        }
        else
        {
            try
            {
                repo = imperium::repo_create("./");
            }
            catch (const std::exception &e)
            {
                std::cerr << e.what() << '\n';
            }
        }
        
    }
    if(strcmp(argv[1], "cat-file") == 0)
    {
        //std::cout  << repo.impDir << "\n";
        repo = imperium::repo_find();
        imperium::object_read(repo, "0a868b7eab9caee8546556201c0d29f50440b0d8");
    }
    return 0;
}