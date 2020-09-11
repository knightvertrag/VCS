#include <iostream>
#include <string.h>
#include <fstream>
#include <sys/stat.h>
#include <filesystem>

namespace fs = std::filesystem;

void init(std::string path)
{
    struct stat buffer;

    path += "/.imperium";
    if (stat(path.c_str(), &buffer) == 0)
    {
        std::cout << "Already initialized as imperium repository"
                  << "\n";
    }
    else
    {
        int created = mkdir(path.c_str(), 0777);
        if (created == 0)
        {   std::string commitLog = path + "/commit.log";
            std::ofstream commit(commitLog.c_str());
            std::string addLog = path + "/add.log";
            std::ofstream add(addLog.c_str());
            add.close();
            commit.close();
            std::cout << "Initialised imperium repository"
                      << "\n";
        }
        else
        {
            std::cout << "Error with creation"
                      << "\n";
        }
    }
}

void add(std::string path, char **argv)
{   
    struct stat buffer;   
    if(stat((path+"/.imperium").c_str(), &buffer)==0){
    std::ofstream addFile;
    addFile.open (path+"/.imperium/add.log");
    if(strcmp(argv[2], ".")==0)
    {
        struct stat s;
        for(auto& p: fs::recursive_directory_iterator(path)){
            if(stat (p.path().c_str(), &s) == 0){
            if( s.st_mode & S_IFREG )
            {
                addFile << p.path() << "-f\n";
                std::cout << "added file: " << p.path() << "\n";
            }
            else if( s.st_mode & S_IFDIR )
            {
                 addFile << p.path() << "-d\n";
                 std::cout << "added directory: " << p.path() << "\n";   
            }
            else{
                continue;
            }
            
            }   
        }

        addFile.close();
    }
    else
    {   struct stat s;
        if(stat ((path+argv[2]).c_str(), &s) == 0){
            path += argv[2];
            if( s.st_mode & S_IFDIR )
            {
                 for(auto& p: fs::recursive_directory_iterator(path)){
                    if(stat (p.path().c_str(), &s) == 0){
                        if( s.st_mode & S_IFREG )
                        {
                            addFile << p.path() << "-f\n";
                            std::cout << "added file: " << p.path() << "\n";
                        }
                        else if( s.st_mode & S_IFDIR )
                        {
                            addFile << p.path() << "-d\n";
                            std::cout << "added directory: " << p.path() << "\n";   
                        }
                        else{
                            continue;
                        }
                    }   
                }
            }
            else if( s.st_mode & S_IFREG )
            {
                addFile << path+argv[2] << "-f\n";
                std::cout << "added file: "<< path+argv[2]<< "\n";
                addFile.close();
            }
            else
            {
                std::cout << "path is not a file.\n";
            }
            
        }
        else
        {
                std::cout << "file doesn't exist, kindly check.\n";
                addFile.close();
        }
        
    }
    std::cout << "add used\n";
    }
}

void commit()
{
    std::cout << "commit used\n";
}

int main(int argc, char **argv)
{
    const char *dir = getenv("dir");
    if (strcmp(argv[1], "init") == 0)
    {   
        init(dir);
    }
    if (strcmp(argv[1], "add") == 0)
    {
        add(dir,argv);
    }
    else if (strcmp(argv[1], "commit") == 0)
    {
        commit();
    }
}
