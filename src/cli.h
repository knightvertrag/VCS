#ifndef IMPERIUM_CLI_H_
#define IMPERIUM_CLI_H_

#include <filesystem>
#include "repository.h"

namespace fs = std::filesystem;

namespace imperium
{
    /**
     * initialize an imperium repository
     * 
     * @param path Optional. Path to worktree
    */
    inline void init(fs::path path = "./")
    {
        try
        {
            imperium::repo_create(path);
        }
        catch (const std::exception &e)
        {
            std::cerr << e.what() << '\n';
        }
    }
}

#endif // IMPERIUM_CLI_H_