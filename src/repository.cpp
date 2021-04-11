#include "repository.hpp"

#include <INIReader.h>
#include "configParser.h"
#include <assert.h>

#include <iostream>
#include <vector>
#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;

imperium::Repository::Repository(fs::path path, bool force)
{
    std::cout << "Repo starting\n";
    worktree = path;
    impDir = fs::current_path() / ".imperium/";
    // if (!(force && fs::exists(impDir)))
    // {
    //     std::string err = "Not an imperium repository" + path.generic_string();
    //     throw err;
    // }
    // INIReader reader("config");
    // conf = reader;
    fs::path cf = repo_file(*this, {"config"});
    if (fs::exists(cf))
    {
        INIReader reader("cf");
        conf = reader;
    }
    else if (!force)
        throw "Configuration file missing";
    if (!force)
    {
        int vers = conf.GetInteger("core", "repositoryformatversion", -1);
        if (vers != 0)
        {
            std::string err = "Unsupported repositoryformatversion" + vers;
        }
    }
}

/**
 * Compute Path under Repo's impdir
 * 
 * @param repo base repository
 * @param paths to be appended
 * @return Constructed Path
 * */
fs::path imperium::repo_path(Repository &repo, std::vector<fs::path> paths)
{
    fs::path joined = repo.impDir;
    for (auto p : paths)
        joined /= p;
    return joined;
}

/**
 * Same as repo_path, but create directory at leaf of paths if absent and mkdir == true.
 * 
 * @param repo base repository
 * @param paths vector representing directory tree
 * @return Path to directory created
*/
fs::path imperium::repo_dir(Repository &repo, std::vector<fs::path> paths, bool mkdir)
{
    fs::path path = repo_path(repo, paths);
    if (fs::exists(path))
    {
        if (fs::is_directory(path))
            return path;
        else
            throw "Not a directory" + path.generic_string();
    }
    if (mkdir)
    {
        fs::create_directories(path);
        return path;
    }
}

/**
 * Same as repo_path, but create file at leaf of paths if absent.
 * 
 * @param repo base repository
 * @param paths vector representing directory tree
 * @return Path to file created
*/
fs::path imperium::repo_file(Repository &repo, std::vector<fs::path> paths, bool mkdir)
{
    //get rid of HEAD
    //paths.pop_back();
    try
    {
        fs::path path = repo_dir(repo, paths, mkdir);
        return path;
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
    }
}

/**
 * Create a new Repository at path
 * 
 * @param path Path to create the repo at
*/
imperium::Repository imperium::repo_create(fs::path path)
{
    Repository repo = Repository(path, true);

    //Make sure the path either doesn't exist or is an empty directory
    if (fs::exists(repo.worktree))
    {
        if (!fs::is_directory(repo.worktree))
            throw path.generic_string() + "is not a directory";
        // if (!fs::is_empty(repo.worktree))
        //     throw path.generic_string() + "is not empty";
    }
    else
        fs::create_directories(repo.worktree);

    repo_dir(repo, {"branches"}, true);
    repo_dir(repo, {"objects"}, true);
    repo_dir(repo, {"refs", "tags"}, true);
    repo_dir(repo, {"branches", "heads"}, true);

    std::ofstream desc_file(repo_file(repo, {"description"}));
    desc_file << "Unnamed repository; edit this file 'description' to name the repository.\n";
    desc_file.close();

    std::ofstream head_file(repo_file(repo, {"HEAD"}));
    head_file << "ref: refs/heads/master\n";
    head_file.close();

    std::ofstream config_file(repo_file(repo, {"config"}));
    std::vector<std::string> config_lines{
        "[core]\n",
        "repositoryformatversion=2\n",
        "filemode=false\n",
        "bare=false\n",
    };
    Configparser::initialize_config(config_file, config_lines);
    config_file.close();

    return repo;
}