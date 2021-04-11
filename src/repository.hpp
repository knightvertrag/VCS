#ifndef IMPERIUM_REPOSITORY_H_
#define IMPERIUM_REPOSITORY_H_

#include <iostream>
#include <vector>
#include <filesystem>
#include <INIReader.h>

namespace fs = std::filesystem;

namespace imperium
{
    class Repository
    {
    public:
        fs::path worktree;
        fs::path impDir;
        INIReader conf;
        Repository(fs::path path, bool force);
    };

    fs::path repo_path(Repository &repo, std::vector<fs::path> paths);
    fs::path repo_file(Repository &repo, std::vector<fs::path> paths, bool mkdir = false);
    fs::path repo_dir(Repository &repo, std::vector<fs::path> paths, bool mkdir = false);
    Repository repo_create(fs::path path);
};

#endif