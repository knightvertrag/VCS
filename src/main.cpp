#include <iostream>
#include <string.h>

void init()
{
    std::cout << "init used\n";
}

void add()
{
    std::cout << "add used\n";
}

void commit()
{
    std::cout << "commit used\n";
}

int main(int argc, char **argv)
{
    //std::cout << "Hello from Fenix!" << std::endl;
    if (strcmp(argv[1], "init") == 0)
    {
        init();
    }
    if (strcmp(argv[1], "add") == 0)
    {
        add();
    }
    else if (strcmp(argv[1], "commit") == 0)
    {
        commit();
    }
}
