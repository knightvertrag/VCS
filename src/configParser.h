#pragma once

#include <fstream>
#include <vector>
#include <map>
#include <filesystem>

namespace fs = std::filesystem;

namespace cparse
{
    class ConfigFile
    {

    public:
        fs::path path;
        std::map<std::string, std::map<std::string, std::string>> data;
        ConfigFile(fs::path path)
        {
            std::fstream file(path, std::ios::in);
            std::string line;
            std::string section, key, value;
            while (std::getline(file, line, '\n'))
            {
                if (line[0] == '[')
                {
                    // section started
                    section = line.substr(1, line.length() - 2);
                }
                else if (line[0] == '\t')
                {
                    // key-value element
                    key = line.substr(1, line.find(' ') - 1);
                    value = line.substr(line.find('=') + 2);
                    data[section][key] = value;
                }
            }
            file.close();
        }
    };
} // namespace cparse
