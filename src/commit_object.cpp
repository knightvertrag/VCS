#include "commit_object.h"
#include "tree_object.h"
#include "imperium_object.h"
#include "configParser.h"

#include <fstream>
#include <chrono>
#include <sstream>

using namespace imperium;

std::string Commitobject::serialize()
{
    std::vector<std::string> order = {"tree", "parent", "author", "committer", "message"};
    std::map<std::string, std::string> mp;
    mp["tree"] = this->tree;
    mp["parent"] = this->parent;
    mp["author"] = this->author;
    mp["committer"] = this->committer;
    mp["message"] = this->message;
    std::string res = kv_serialize(mp, order);
    this->data = res;
    return res;
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
    if (parent != "")
        std::cout << "parent " << parent << "\n";
    std::cout << "author " << author << "\n";
    std::cout << "committer " << committer << "\n\n";
    std::cout << message << "\n\n";
}

Commitobject::Commitobject(Repository repo, std::string data) : Impobject(repo, data, "commit")
{
    deserialize(data);
}

Commitobject::Commitobject(std::string message) : Impobject(repo_find(), "", "commit")
{
    auto config_file = cparse::ConfigFile(repo.worktree / ".imperium" / "config");
    std::string author = config_file.data["user"]["name"];
    std::string email = config_file.data["user"]["email"];
    auto p1 = std::chrono::system_clock::now();
    auto time_stamp = std::chrono::duration_cast<std::chrono::seconds>(p1.time_since_epoch()).count(); // @Todo: Add the timezone offset
    std::string tree_id = Treeobject::construct_tree({"folder1/folder1_2/file4.py"});                  // When INDEX is added this will be dynamic
    this->author = author + " " + "<" + email + ">" + " " + std::to_string(time_stamp);
    this->committer = author + " " + "<" + email + ">" + " " + std::to_string(time_stamp);
    this->tree = tree_id;
    std::ifstream HEAD_in((repo.worktree / ".imperium" / "HEAD"), std::ios::in);
    std::string line;
    while (std::getline(HEAD_in, line))
    {
        this->parent = line;
    }
    HEAD_in.close();
    std::ofstream HEAD_out((repo.worktree / ".imperium" / "HEAD"), std::ios::trunc);
    HEAD_out << tree_id;
    HEAD_out.close();
    this->message = message;
}