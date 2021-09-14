#pragma once

#include <fstream>
#include <vector>
#include <filesystem>

namespace fs = std::filesystem;

/**
 * Functions to work with the config file
 * @todo Convert this to a proper map parser
*/
class Configparser
{
private:
    fs::path file_path;
    std::fstream file;

public:
    /**
     * initialize the config file with a vector
     * @param config_file Config file to initialize
    */
    static void initialize_config(std::ofstream &config_file, std::vector<std::string> vec)
    {
        for (auto l : vec)
            config_file << l;
    }

    /**
     * Read the entire config file
     * @param config_file The config file istream
     * @return vector of lines
    */
    std::vector<std::string> read_from_config(std::ifstream &config_file)
    {
        std::vector<std::string> res;
        std::string line;
        while (std::getline(config_file, line))
        {
            res.push_back(line);
        }
        return res;
    }
};
