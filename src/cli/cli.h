#ifndef IMPERIUM_CLI_H_
#define IMPERIUM_CLI_H_

#include <filesystem>
#include <CLI11.hpp>
#include "repository.h"
#include "imperium_object.h"

namespace fs = std::filesystem;

namespace imperium
{
    /**
     * initialize an imperium repository
     * 
     * @param path Optional. Path to worktree
    */
    inline Repository init(fs::path path = "./")
    {
        try
        {
            return imperium::repo_create(path);
        }
        catch (const std::exception &e)
        {
            std::cerr << e.what() << '\n';
        }
    }
    /**
     * Cat object content and type
     * 
     * @param t Flag to specify entire content or just type
    */
    inline void cat_file(char t)
    {
        if (t == 't')
        {
            Repository repo = repo_find();
            auto blob = object_read(repo, "");
        }
    }
}

#endif // IMPERIUM_CLI_H_