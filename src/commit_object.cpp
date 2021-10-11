#include "commit_object.h"
#include "tree_object.h"
#include "imperium_object.h"
#include "configParser.h"
#include "refs.h"

#include <fstream>
#include <chrono>
#include <sstream>
#include <ctime>

using namespace imperium;

std::string time_stamp()
{
    std::time_t rawtime = std::time(NULL);
    std::tm raw_tm{};
    raw_tm = *std::localtime(&rawtime);
    std::tm tm{};
    tm = *std::gmtime(&rawtime);
    tm.tm_isdst = -1;
    std::mktime(&tm);
    std::stringstream ss;
    ss << std::put_time(&raw_tm, "%s ");
    ss << std::put_time(&tm, "%z");
    return ss.str();
}

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
    std::string tree_id = Treeobject::construct_tree({"."}); // When INDEX is added this will be dynamic
    this->author = author + " " + "<" + email + ">" + " " + time_stamp();
    this->committer = author + " " + "<" + email + ">" + " " + time_stamp();
    this->tree = tree_id;
    auto refs = Refs(repo.impDir);
    this->parent = refs.read_head();
    refs.update_head(tree_id);
    this->message = message;
}