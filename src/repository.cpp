#include "repository.h"

#include "configParser.h"

#include <iostream>
#include <vector>
#include <fstream>
#include <filesystem>

namespace fs = std::filesystem;

imperium::Repository::Repository(fs::path path, bool force)
{
    worktree = path;
    impDir = path / ".imperium";
    if (!(force || fs::exists(impDir)))
    {
        std::cerr << "Not an imperium repository" + path.generic_string();
        exit(-1);
    }
    // TODO: Add checks for config file(Low priority)
}

fs::path imperium::repo_path(Repository &repo, std::vector<fs::path> paths)
{
    fs::path joined = repo.impDir;
    for (auto p : paths)
        joined /= p;
    return joined;
}

fs::path imperium::repo_dir(Repository &repo, std::vector<fs::path> paths, bool mkdir)
{
    fs::path path = repo_path(repo, paths);
    if (fs::exists(path))
    {
        if (fs::is_directory(path))
            return path;
        else
        {
            std::cerr << "Not a directory" + path.generic_string();
            exit(-1);
        }
    }
    if (mkdir)
    {
        fs::create_directories(path);
        return path;
    }
}

fs::path imperium::repo_file(Repository &repo, std::vector<fs::path> paths, bool mkdir)
{
    std::string filename = paths.back();
    //get rid of filename
    paths.pop_back();
    try
    {
        //get directory path to file
        fs::path path = repo_dir(repo, paths, mkdir);
        //append file
        return path / filename;
    }
    catch (const std::exception &e)
    {
        std::cerr << e.what() << '\n';
    }
}

imperium::Repository imperium::repo_create(fs::path path)
{
    Repository repo = Repository(path, true);

    //Make sure the path either doesn't exist or is an empty directory
    if (fs::exists(repo.impDir))
    {
        if (!fs::is_directory(repo.impDir))
            throw path.generic_string() + "is not a directory";
        if (!fs::is_empty(repo.impDir))
            throw path.generic_string() + "is not empty";
    }
    else
        fs::create_directories(repo.worktree);

    //make all the subdirectories and files under .imperium
    repo_dir(repo, {"branches"}, true);
    repo_dir(repo, {"objects"}, true);
    repo_dir(repo, {"refs", "tags"}, true);
    repo_dir(repo, {"refs", "heads"}, true);

    //fill in the default info inside files
    std::ofstream desc_file(repo_file(repo, {"description"}));
    desc_file << "Unnamed repository; edit this file 'description' to name the repository.\n";
    desc_file.close();

    std::ofstream head_file(repo_file(repo, {"HEAD"}));
    head_file << "ref: refs/heads/master\n";
    head_file.close();

    std::ofstream config_file(repo_file(repo, {"config"}));
    std::vector<std::string> config_lines{
        "[core]\n",
        "\trepositoryformatversion = 2\n",
        "\tfilemode = false\n",
        "\tbare = false\n",
    };
    Configparser::initialize_config(config_file, config_lines);
    config_file.close();

    return repo;
}

imperium::Repository imperium::repo_find(fs::path path, bool required)
{
    path = fs::canonical(path);
    if (fs::is_directory(path / ".imperium"))
        return Repository(path, false);

    fs::path parent = fs::canonical(path / "..");

    if (parent == path)
    {
        if (required)
        {
            std::cerr << "Not an Imperium Repository\n";
            exit(-1);
        }
    }
    return repo_find(parent, required);
}
