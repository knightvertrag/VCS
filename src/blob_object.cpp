#include "blob_object.h"
#include "imperium_object.h"

#include <fstream>
#include <filesystem>
#include <iostream>
#include <memory>
#include <sstream>

namespace fs = std::filesystem;

imperium::Blobobject::Blobobject(imperium::Repository repo, std::string data) : imperium::Impobject::Impobject(repo, data, "blob")
{
    deserialize(data);
}

std::string imperium::Blobobject::serialize()
{
    return this->data;
}

void imperium::Blobobject::deserialize(std::string data)
{
    this->data = data;
}

void imperium::Blobobject::pretty_print()
{
    std::cout << data.substr(data.find('\0')) << "\n";
}

std::string imperium::Blobobject::blob_from_file(fs::path path)
{
    std::ifstream file_to_hash(path, std::ios::in);
    std::stringstream blob_buffer;
    blob_buffer << file_to_hash.rdbuf();
    std::shared_ptr<Blobobject> blob = std::make_shared<Blobobject>(repo_find(), blob_buffer.str());
    std::string sha = object_write(*blob, true);
    file_to_hash.close();
    return sha;
}