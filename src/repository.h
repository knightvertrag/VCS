#pragma once

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
        Repository() {}
        Repository(fs::path path, bool force = false);
    };

    /**
    * Compute Path under Repo's impdir
    * 
    * @param repo base repository
    * @param paths to be appended
    * @return Constructed Path
    * */
    fs::path repo_path(Repository &repo, std::vector<fs::path> paths);

    /**
     * Same as repo_path, but create directories till leaf of paths if absent.
     * 
     * @param repo base repository
     * @param paths vector representing directory tree
     * @return Path to file created
    */
    fs::path repo_file(Repository &repo, std::vector<fs::path> paths, bool mkdir = false);

    /**
     * Same as repo_path, but create directory at leaf of paths if absent and mkdir == true.
     * 
     * @param repo base repository
     * @param paths vector representing directory tree
     * @return Path to directory created
    */
    fs::path repo_dir(Repository &repo, std::vector<fs::path> paths, bool mkdir = false);

    /**
     * Create a new Repository at path
     * 
     * @param path Path to create the repo at
    */
    Repository repo_create(fs::path path);

    /**
     * Find the Imperium repo closest to path
     * 
     * @param path Path to find the repo in
     * @param required Raise error if not found
     * @return Repository object found
    */
    Repository repo_find(fs::path path = "./", bool required = true);
};
