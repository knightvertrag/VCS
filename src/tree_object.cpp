#include "tree_object.h"

#include <filesystem>
#include <boost/algorithm/hex.hpp>

using namespace imperium;
namespace fs = std::filesystem;

std::pair<uint32_t, TreeLeaf> parse_one(std::string raw, uint32_t start)
{
    uint32_t space_pos = raw.find(' ', start);
    uint32_t null_pos = raw.find('\0', space_pos);
    std::string mode = raw.substr(start, space_pos - start);
    fs::path path = raw.substr(space_pos + 1, null_pos - space_pos + 1);
    std::string sha = boost::algorithm::hex(raw.substr(null_pos + 1, 21));
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
        std::cout << i.mode << " " << i.sha << " " << i.path << "\n";
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
    return this->data;
}