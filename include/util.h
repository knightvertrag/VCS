#pragma once

#include <string>

namespace vertrag
{
    namespace algorithm
    {
        // Convert string of chars to its representative string of hex numbers
        inline std::string hex(const std::string &str, bool capital = false)
        {
            std::string hexstr;
            hexstr.resize(str.size() * 2);
            const size_t a = capital ? 'A' - 1 : 'a' - 1;

            for (size_t i = 0, c = str[0] & 0xFF; i < hexstr.size(); c = str[i / 2] & 0xFF)
            {
                hexstr[i++] = c > 0x9F ? (c / 16 - 9) | a : c / 16 | '0';
                hexstr[i++] = (c & 0xF) > 9 ? (c % 16 - 9) | a : c % 16 | '0';
            }
            return hexstr;
        }

        // Convert string of hex numbers to its equivalent char-stream
        inline std::string unhex(const std::string &hexstr)
        {
            std::string str;
            str.resize((hexstr.size() + 1) / 2);

            for (size_t i = 0, j = 0; i < str.size(); i++, j++)
            {
                str[i] = (hexstr[j] & '@' ? hexstr[j] + 9 : hexstr[j]) << 4, j++;
                str[i] |= (hexstr[j] & '@' ? hexstr[j] + 9 : hexstr[j]) & 0xF;
            }
            return str;
        }
    } // namespace algorithm
} // namespace vertrag
