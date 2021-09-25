#include "tree_object.h"
#include "imperium_object.h"
#include "repository.h"

#include <filesystem>
#include <boost/algorithm/hex.hpp>
#include <map>

using namespace imperium;
namespace fs = std::filesystem;

std::pair<uint32_t, TreeLeaf> parse_one(std::string raw, uint32_t start)
{
    uint32_t space_pos = raw.find(' ', start);
    uint32_t null_pos = raw.find('\0', space_pos);
    std::string mode = raw.substr(start, space_pos - start);
    if (mode.length() < 6)
    {
        mode = "0" + mode;
    }
    fs::path path = raw.substr(space_pos + 1, null_pos - space_pos);
    std::string sha = boost::algorithm::hex(raw.substr(null_pos + 1, 20));
    std::transform(sha.begin(), sha.end(), sha.begin(), ::tolower);
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
    data = data.substr(data.find('\0'));
    this->items = parse_tree(data);
}

std::string Treeobject::serialize()
{
    /*@Todo: Properly Serialize*/
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

std::shared_ptr<Treeobject> Treeobject::construct_tree(std::vector<fs::path> paths)
{
    std::vector<TreeLeaf> entries;
    auto repo = repo_find();
    for (auto path : paths)
    {
        auto absolute_path = repo.worktree / path;
        if (absolute_path == (repo.worktree / ".imperium"))
            continue;
        if (fs::is_directory(path))
        {
            //recursively contruct tree for this directory
        }
        else
        {
            // Create blobs from content
            std::string sha = Blobobject::blob_from_file(absolute_path);
            //create a map of path-><mode, object, sha>, mode shoudl be dynamic
            entries.push_back(TreeLeaf("100644", path, sha));
        }
    }
    auto tree = std::make_shared<Treeobject>(entries, repo, "");
    return tree;
}