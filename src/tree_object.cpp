#include "tree_object.h"
#include "imperium_object.h"
#include "blob_object.h"
#include "repository.h"
#include "index.h"

#include <stddef.h>
#include <filesystem>
#include <memory>
#include <map>
#include <ordered_map.h>
#include <variant>
#include <utility>
#include <util.h>

using namespace imperium;
namespace fs = std::filesystem;

/**
 * Get the mode for path. However only 100644 and 100755 are allowed by Git for files, and 40000 for directories.
 * @return mode reduced to the ones allowed
 */
std::string file_mode(fs::path path)
{
    auto perms = fs::status(path).permissions();
    if (fs::is_directory(path))
        return "40000";
    if ((perms & fs::perms::owner_exec) != fs::perms::none)
        return "100755";
    else
        return "100644";
}

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
    size_t space_pos = raw.find(' ', start);
    size_t null_pos = raw.find('\0', space_pos);
    std::string mode = raw.substr(start, space_pos - start);
    fs::path path = raw.substr(space_pos + 1, null_pos - space_pos);
    std::string sha = vertrag::algorithm::hex(raw.substr(null_pos + 1, 20));
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
    for (auto &i : _items)
    {
        // find the object with the current i.sha to get type
        auto repo = repo_find();
        auto obj = object_read(repo, i.sha);
        std::cout << ((i.mode.length() < 6) ? ("0" + i.mode) : i.mode);
        std::cout << " " << obj->type << " " << i.sha << "\t" << i.path.generic_string() << "\n";
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
    this->_items = parse_tree(data);
}

std::string Treeobject::serialize()
{
    std::string res;
    for (auto item : _items)
    {
        res += item.mode;
        res += " ";
        res += item.path.filename();
        res += '\0';
        res += vertrag::algorithm::unhex(item.sha);
    }
    data = res;
    return res;
}

/**
 * @return vector of all parent directories of path in descending order
 */
std::vector<fs::path> parent_dirs(fs::path path)
{
    auto repo = repo_find();
    std::vector<fs::path> res;
    while (path != repo.worktree)
    {
        path = path.parent_path();
        if (path != repo.worktree)
            res.push_back(fs::relative(path, repo.worktree));
    }
    return res;
}

void Treeobject::add_entry(std::vector<fs::path> &parents, TreeLeaf &item)
{
    auto repo = repo_find();
    item.path = fs::relative(item.path, repo.worktree);
    if (parents.empty())
        _entries[item.path.generic_string()] = item;
    else
    {
        auto b_parents = parents;
        if (!_entries.find(parents.back()))
        {
            auto tree = new Treeobject();
            parents.pop_back();
            tree->add_entry(parents, item);
            _entries[b_parents.back().generic_string()] = *tree;
            delete tree;
        }
        else
        {
            auto tree = std::get<1>(_entries[parents.back()]);
            parents.pop_back();
            tree.add_entry(parents, item);
            _entries[b_parents.back().generic_string()] = tree;
        }
    }
}

std::string Treeobject::traverse()
{
    auto repo = repo_find();
    for (auto &e : _entries)
    {
        if (e.second().index() == 1)
        {
            auto tree = std::get<1>(e.second());
            auto sha = tree.traverse();
            _items.push_back(TreeLeaf("40000", e.first(), sha));
        }
        else
        {
            auto leaf = std::get<0>(e.second());
            _items.push_back(leaf);
        }
    }
    auto tree = std::make_shared<Treeobject>(_items, repo, "");
    auto sha = object_write(*tree);
    return sha;
}

Treeobject *Treeobject::build(std::vector<TreeLeaf> &__entries)
{
    auto sorter = [](TreeLeaf &a, TreeLeaf &b)
    {
        return a.path.generic_string() < b.path.generic_string();
    };
    std::sort(__entries.begin(), __entries.end(), sorter);
    auto root = new Treeobject();
    for (auto e : __entries)
    {
        auto pdirs = parent_dirs(e.path);
        root->add_entry(pdirs, e);
    }
    return root;
}

std::string Treeobject::construct_tree()
{
    Repository repo = repo_find();
    std::vector<TreeLeaf> entries;
    Index index(repo.impDir / "index");
    index.read_index();
    for (auto &[key, entry] : index._entries)
    {
        auto path = repo.worktree / entry._path;
        entries.push_back(TreeLeaf(file_mode(path), path, entry._sha));
    }
    auto root = Treeobject::build(entries);
    std::string sha = root->traverse();
    delete root;
    return sha;
}