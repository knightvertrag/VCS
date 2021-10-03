#include "tree_object.h"
#include "imperium_object.h"
#include "blob_object.h"
#include "repository.h"

#include <filesystem>
#include <memory>
#include <boost/algorithm/hex.hpp>
#include <map>

using namespace imperium;
namespace fs = std::filesystem;

/**
 * Parse one entry in the tree object. Since entries do not have delimiters we parse one entry and return the starting index of the next one.
 * 
 * @param raw entire tree data
 * @param start starting index of the current entry
 * @return constructed TreeLeaf and starting index of next entry
*/
std::pair<uint32_t, TreeLeaf> parse_one(std::string &raw, uint32_t start)
{
    /**
     * Each entry looks like => <mode> <relative path>\0<sha>
    */
    uint32_t space_pos = raw.find(' ', start);
    uint32_t null_pos = raw.find('\0', space_pos);
    std::string mode = raw.substr(start, space_pos - start);
    if (mode.length() < 6)
    {
        mode = "0" + mode;
    }
    fs::path path = raw.substr(space_pos + 1, null_pos - space_pos);
    std::string sha = boost::algorithm::hex_lower(raw.substr(null_pos + 1, 20));
    return {null_pos + 21, TreeLeaf(mode, path, sha)};
}

std::vector<TreeLeaf> parse_tree(std::string raw)
{
    uint32_t pos = 0;
    uint32_t mx = raw.length();
    std::vector<TreeLeaf> res;
    while (pos < mx)
    {
        auto pair = parse_one(raw, pos);
        pos = pair.first;
        res.push_back(pair.second);
    }
    return res;
}

void Treeobject::pretty_print()
{
    for (auto &i : items)
    {
        // find the object with the current i.sha to get type
        auto repo = repo_find();
        auto obj = object_read(repo, i.sha);
        std::cout << i.mode << " " << obj->type << " " << i.sha << "\t" << i.path.generic_string() << "\n";
        delete obj;
    }
}

Treeobject::Treeobject(Repository repo, std::string data) : Impobject(repo, data, "tree")
{
    deserialize(data);
}

void Treeobject::deserialize(std::string data)
{
    data = data.substr(data.find('\0') + 1);
    this->items = parse_tree(data);
}

std::string Treeobject::serialize()
{
    std::string res;
    for (auto item : items)
    {
        res += item.mode;
        res += " ";
        res += item.path;
        res += '\0';
        res += boost::algorithm::unhex(item.sha);
    }
    data = res;
    return res;
}

std::string Treeobject::construct_tree(std::vector<fs::path> paths)
{
    // @Todo: Currently this function starts constructing the tree directly from the paths provided.
    // So that means if we commit file.txt like - commit folder1/folder2/file.txt
    // then only one tree object will be created pointing directly to folder1/folder2/file.txt
    // even though individual tree objects for folder1 and folder2 should be created first.
    // So we need it to always start construction at root
    std::vector<TreeLeaf> entries;
    auto repo = repo_find();
    for (auto path : paths)
    {
        auto absolute_path = (path == ".") ? repo.worktree : repo.worktree / path;
        if (absolute_path == repo.impDir || fs::is_empty(absolute_path))
            continue;
        if (fs::is_directory(absolute_path))
        {
            for (auto &p : fs::directory_iterator(absolute_path))
            {
                if (fs::is_empty(p.path()) || (p == (repo.impDir)))
                    continue;
                if (fs::is_directory(p.path()))
                {
                    std::string sha = construct_tree({fs::relative(p.path(), repo.worktree)});
                    entries.push_back(TreeLeaf("100644", fs::relative(p.path(), absolute_path), sha));
                }
                else
                {
                    std::string sha = Blobobject::blob_from_file(p);
                    entries.push_back(TreeLeaf("100644", fs::relative(p.path(), absolute_path), sha));
                }
            }
        }
        else
        {
            // Create blobs from content
            std::string sha = Blobobject::blob_from_file(absolute_path);
            // @Todo: mode should be dynamic
            entries.push_back(TreeLeaf("100644", path, sha));
        }
    }
    auto tree = std::make_shared<Treeobject>(entries, repo, "");
    std::string sha = object_write(*tree);
    return sha;
}