#include <filesystem>
#include <fstream>
#include <zlib.h>
#include <iostream>
#include <algorithm>
#include <iterator>
#include <boost/iostreams/filtering_streambuf.hpp>
#include <boost/iostreams/copy.hpp>
#include <boost/iostreams/filter/zlib.hpp>

void write_object()
{
    using namespace std;
    using namespace boost::iostreams;
    std::stringstream compressed;
    std::stringstream decompressed;
    string data = "Hello World";
    decompressed << data;
    boost::iostreams::filtering_streambuf<boost::iostreams::input> out;
    out.push(boost::iostreams::zlib_compressor());
    out.push(decompressed);
    boost::iostreams::copy(out, compressed);
    cout << compressed.str();
    ofstream file("hello", ios::binary | ios::out);
    file << compressed.str();
    file.close();
}

void read_object()
{
    using namespace std;
    using namespace boost::iostreams;
    std::stringstream compressed;
    std::stringstream decompressed;
    ifstream file("/home/anurag/Code/cpp/VCS/.git/objects/7b/bc84a9b8f7aa4f1ef3f78128bd8fd5766eaf1f", ios::binary | ios::in);
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
    ofstream outfile("hello", ios::out);
    outfile << raw;
}

int main()
{
    read_object();
    return 0;
}