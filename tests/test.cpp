#include <filesystem>
#include <fstream>
#include <zlib.h>
#include <iostream>
#include <algorithm>
#include <iterator>
#include <boost/iostreams/filtering_streambuf.hpp>
#include <boost/iostreams/copy.hpp>
#include <boost/iostreams/filter/zlib.hpp>
#include <boost/compute/detail/sha1.hpp>

void write_object()
{
    using namespace std;
    using namespace boost::iostreams;
    std::stringstream compressed;
    std::stringstream decompressed;
    string data = "blob 12\x00simple text";
    decompressed << data;
    boost::iostreams::filtering_streambuf<boost::iostreams::input> out;
    out.push(boost::iostreams::zlib_compressor());
    out.push(decompressed);
    boost::iostreams::copy(out, compressed);
    // cout << compressed.str();
    std::string sha = boost::compute::detail::sha1(compressed.str());
    cout << sha << "\n";
    ofstream file(sha, ios::binary | ios::out);
    file << compressed.str();
    file.close();
}

void read_object()
{
    using namespace std;
    using namespace boost::iostreams;
    std::stringstream compressed;
    std::stringstream decompressed;
    ifstream file("/home/anurag/Code/cpp/VCS/.git/objects/0e/ead9e48e5ee82e86f5a40ef02e8ab81b691e2c", ios::binary | ios::in);
    //ostringstream sout;
    copy(istreambuf_iterator<char>(file),
         istreambuf_iterator<char>(),
         ostreambuf_iterator<char>(compressed));
    boost::iostreams::filtering_streambuf<boost::iostreams::input> in;
    in.push(boost::iostreams::zlib_decompressor());
    in.push(compressed);
    boost::iostreams::copy(in, decompressed);
    string raw = decompressed.str();
    //cout << raw << "\n";
    string obj_type = raw.substr(0, raw.find(" "));
    ofstream outfile("hello", ios::out | ios::binary);
    outfile << raw;
    ifstream hellofile("hello", ios::binary | ios::in);
    char reading[50];
    hellofile.read(reading, 50);
    for (int i = 0; i < 50; i++)
        cout << reading[i];
}

int main()
{
    read_object();
    return 0;
}