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
    ifstream file("/home/anurag/Code/cpp/VCS/.git/objects/1a/a01e257d1da196915a35c083c0bdf56d5c13e9", ios::binary | ios::in);
    //ostringstream sout;
    copy(istreambuf_iterator<char>(file),
         istreambuf_iterator<char>(),
         ostreambuf_iterator<char>(compressed));
    boost::iostreams::filtering_streambuf<boost::iostreams::input> in;
    in.push(boost::iostreams::zlib_decompressor());
    in.push(compressed);
    boost::iostreams::copy(in, decompressed);
    string raw = decompressed.str();
    cout << raw << "\n";
    string obj_type = raw.substr(0, raw.find(" "));
    ofstream outfile("hello", ios::out | ios::binary);
    outfile << raw;
}

int main()
{
    read_object();
    return 0;
}