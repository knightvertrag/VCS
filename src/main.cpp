#include <iostream>
#include "repository.h"

int main(int argc, char *argv[])
{
    if (strcmp(argv[1], "init") == 0)
    {
        if (argc == 3)
            try
            {
                imperium::repo_create(argv[3]);
            }
            catch (const std::exception &e)
            {
                std::cerr << e.what() << '\n';
            }
        else
            try
            {
                imperium::repo_create("./");
            }
            catch (const std::exception &e)
            {
                std::cerr << e.what() << '\n';
            }
    }
    return 0;
}