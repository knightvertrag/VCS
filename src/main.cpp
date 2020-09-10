#include <iostream>
#include <string.h>
#include <sys/stat.h>

void init(std::string path)
{
    struct stat buffer;

    path+="/.imperium";
    if(stat(path.c_str(), &buffer)==0)
    {
        std::cout<<"Already initialized as imperium repository"<<"\n";
    }
    else
    {
        int created=mkdir(path.c_str(),0777);
        if(created==0)
        {
            std::cout<<"Initialised imperium repository"<<"\n";
        }
        else
        {
            std::cout<<"Error with creation"<<"\n";
        }
    }
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
    std::string dir=getenv("dir");
    if (strcmp(argv[1], "init") == 0)
    {
        init(dir);
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
