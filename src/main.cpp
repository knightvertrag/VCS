#include <iostream>
#include "repository.hpp"

int main(int argc, char *argv[])
{
    std::cout << argc << "\n";
    if(strcmp(argv[1], "init") == 0)
    {
        if(argc == 3)
            imperium::repo_create(argv[3]);
        else
            imperium::repo_create("./");
    }
    return 0;
}