#include "commit_object.h"
#include "tree_object.h"
#include "imperium_object.h"
#include "configParser.h"

using namespace imperium;

Commitobject::Commitobject(Repository repo, std::string data) : Impobject(repo, data, "commit")
{
    deserialize(data);
}

std::string Commitobject::serialize()
{
    // @Todo: Properly serialize
    return "";
}

void Commitobject::deserialize(std::string data)
{
    std::string content = data.substr(data.find('\0') + 1);
    auto parsed = kv_parse(content);
    this->message = parsed["message"];
    this->author = parsed["author"];
    this->committer = parsed["committer"];
    this->parent = parsed["parent"];
    this->tree = parsed["tree"];
}

void Commitobject::pretty_print()
{
    std::cout << "tree " << tree << "\n";
    std::cout << "parent " << parent << "\n";
    std::cout << "author " << author << "\n";
    std::cout << "committer " << committer << "\n\n";
    std::cout << message << "\n\n";
}