#pragma once

#include <zlib.h>
#include <fstream>
#include <iostream>
#include <sstream>
#include <cstring>
#include <iomanip>

namespace vertrag
{
    namespace zlib
    {
        std::string compress_string(const std::string &str, int compression_level = 1)
        {
            z_stream zs;
            std::memset(&zs, 0, sizeof(zs));
            if (deflateInit(&zs, compression_level) != Z_OK)
            {
                std::cerr << "deflateInit failed while compressing\n";
                exit(-1);
            }
            zs.next_in = (Bytef *)str.data();
            zs.avail_in = str.size();

            int ret;
            char outputbuffer[50000];
            std::string outstring;

            do
            {
                zs.next_out = reinterpret_cast<Bytef *>(outputbuffer);
                zs.avail_out = sizeof(outputbuffer);

                ret = deflate(&zs, Z_FINISH);

                if (outstring.size() < zs.total_out)
                {
                    outstring.append(outputbuffer, zs.total_out - outstring.size());
                }

            } while (ret == Z_OK);

            deflateEnd(&zs);

            if (ret != Z_STREAM_END)
            {
                std::cerr << "Error during zlib compression: " << ret << "\n";
                exit(-1);
            }
            return outstring;
        }

        std::string decompress_string(const std::string &str)
        {
            z_stream zs;
            memset(&zs, 0, sizeof(zs));

            if (inflateInit(&zs) != Z_OK)
            {
                std::cerr << "inflateInit failed while compressing\n";
                exit(-1);
            }

            zs.next_in = (Bytef *)str.data();
            zs.avail_in = str.size();

            int ret;
            char outputbuffer[50000];
            std::string outstring;

            do
            {
                zs.next_out = reinterpret_cast<Bytef *>(outputbuffer);
                zs.avail_out = sizeof(outputbuffer);

                ret = inflate(&zs, 0);

                if (outstring.size() < zs.total_out)
                {
                    outstring.append(outputbuffer, zs.total_out - outstring.size());
                }
            } while (ret == Z_OK);

            inflateEnd(&zs);

            if (ret != Z_STREAM_END)
            {
                std::cerr << "Error during zlib decompression: " << ret << "\n";
                exit(-1);
            }
            return outstring;
        }
    } // namespace zlib

} // namespace Vertrag
