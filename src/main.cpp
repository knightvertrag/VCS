#include <iostream>
#include <string.h>
#include <fstream>
#include <sys/stat.h>
#include <filesystem>
#include <vector>
#include <algorithm>
#include <utility>
#include <string>

namespace fs = std::filesystem;

std::string root = "";

int ignoreFolder(std::string path, std::vector<std::string> folderList)
{
    auto i = folderList.begin();
    while (i != folderList.end())
    {
        std::string folderName = *i++;
        folderName.pop_back();
        if (path.find(folderName, 0) != std::string::npos)
            return 1;
    }

    return 0;
}

void addToCache(std::string file_path, char type){
    struct stat l;
         if (stat((root + "/.imperium/.add").c_str(), &l) != 0)
             mkdir((root + "/.imperium/.add").c_str(), 0777);

        if(type == 'f'){
            struct stat k;
                 std::string filename = file_path.substr(root.length());
                 std::string filerel = root + "/.imperium/.add" + filename.substr(0, filename.find_last_of('/'));
                 if (stat(filerel.c_str(), &k) != 0)
                 {
                     fs::create_directories(filerel);
                 }

                 fs::copy_file(file_path, root + "/.imperium/.add" + filename, fs::copy_options::overwrite_existing);

        }
        else if(type == 'd'){
                   struct stat k;
                   std::string filename = file_path.substr(root.length());
                   std::string filerel = root + "/.imperium/.add" + filename;
                   std::cout << filerel << "\n";
                    if (stat(filerel.c_str(), &k) != 0)
                        {
                            fs::create_directories(filerel);
                        }
        }

}

int toBeIgnored(std::string path)
{
    std::ifstream readIgnoreFile, readAddLog;
    std::string filename;
    std::vector<std::string> filenames;
    std::vector<std::pair <std::string, char> > addedFilenames;
    std::vector<std::string> ignoreDir;

    readIgnoreFile.open(root + "/.imperiumIgnore");
    readAddLog.open(root + "/.imperium/add.log");

    if (readIgnoreFile.is_open())
    {
        while (!readIgnoreFile.eof())
        {
            std::getline(readIgnoreFile, filename);
            auto i = filename.end();
            i--;
            if (*i == '/')
            {
                ignoreDir.push_back(filename);
            }
            else
                filenames.push_back(root + filename);
        }
    }

    readIgnoreFile.close();

    if (readAddLog.is_open())
    {
        while (!readAddLog.eof())
        {
            std::getline(readAddLog, filename);
            if (filename.length() > 4)
            {
                addedFilenames.push_back(make_pair(filename.substr(1, filename.length() - 4), filename.at(filename.length()-1)));
            }
        }
    }

    readAddLog.close();

    for(auto i= addedFilenames.begin(); i!= addedFilenames.end(); i++){
        std::pair< std::string, char> fileEntry =*i;
        if (path.compare(fileEntry.first)==0)
        {
            addToCache(path, fileEntry.second);
            return 1;
        }
    }

    if (std::find(filenames.begin(), filenames.end(), path) != filenames.end() 
        || ignoreFolder(path, ignoreDir))
        return 1;

    return 0;
}

void init(std::string path)
{
    struct stat buffer;

    if (stat((path + "/.imperium").c_str(), &buffer) == 0)
    {
        std::cout << "Already initialized as imperium repository"
                  << "\n";
    }
    else
    {
        std::string imperiumIgnore = path + "/.imperiumIgnore";
        std::ofstream ignore(imperiumIgnore.c_str());
        path += "/.imperium";
        ignore << ".imperium/\n"
               << ".git/\n"
               << "/.imperiumIgnore"
               << ".node_modules/\n"
               << "/.env\n";
        int created = mkdir(path.c_str(), 0777);
        if (created == 0)
        {
            std::string commitLog = path + "/commit.log";
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


void add(char **argv)
{
    struct stat buffer;
    if (stat((root + "/.imperium").c_str(), &buffer) == 0)
    {
        std::ofstream addFile;

        addFile.open(root + "/.imperium/add.log", std::ios_base::app);

        if (strcmp(argv[2], ".") == 0)
        {
            struct stat s;
            for (auto &p : fs::recursive_directory_iterator(root))
            {
                if (toBeIgnored(p.path()))
                    continue;
                if (stat(p.path().c_str(), &s) == 0)
                {
                    if (s.st_mode & S_IFREG)
                    {
                        addFile << p.path() << "-f\n";
                        addToCache(p.path(), 'f');
                        std::cout << "added file: " << p.path() << "\n";
                    }
                    else if (s.st_mode & S_IFDIR)
                    {
                        addFile << p.path() << "-d\n";
                        addToCache(p.path(), 'd');
                        std::cout << "added directory: " << p.path() << "\n";
                    }
                    else
                    {
                        continue;
                    }
                }
            }
            addFile.close();

        }
        else
        {
            struct stat s;
            std::string path = root;
            if (stat((path + "/" + argv[2]).c_str(), &s) == 0)
            {
                path += "/";
                path += argv[2];
                if (s.st_mode & S_IFDIR)
                {   
                    if(!toBeIgnored(path)){
                        addFile << "\""<< path <<"\""<<"-d\n";
                        addToCache(path, 'd');
                        std::cout << "added directory: " << "\"" << path << "\"" << "\n";
                    }
                    for (auto &p : fs::recursive_directory_iterator(path))
                    {    if (toBeIgnored(p.path()))
                            continue;
                        if (stat(p.path().c_str(), &s) == 0)
                        {
                            if (s.st_mode & S_IFREG)
                            {
                                addFile << p.path() << "-f\n";
                                addToCache(p.path(), 'f');
                                std::cout << "added file: " << p.path() << "\n";
                            }
                            else if (s.st_mode & S_IFDIR)
                            {
                                addFile << p.path() << "-d\n";
                                addToCache(p.path(), 'd');
                                std::cout << "added directory: " << p.path() << "\n";
                            }
                            else
                            {
                                continue;
                            }
                        }
                    }
                }
                else if (s.st_mode & S_IFREG)
                {
                    if (!toBeIgnored(path))
                    {
                        addFile << "\""<<path<<"\""<< "-f\n";
                        addToCache(path, 'f');
                        std::cout << "added file: " << "\""<<path<<"\""<< "\n";
                        addFile.close();
                    }
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
    root = dir;
    if (strcmp(argv[1], "init") == 0)
    {
        init(dir);
    }
    if (strcmp(argv[1], "add") == 0)
    {
        add(argv);
    }
    else if (strcmp(argv[1], "commit") == 0)
    {
        commit();
    }
}
