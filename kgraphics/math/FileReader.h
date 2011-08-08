#pragma once 

#include <fstream>

namespace gfx {

class FileReader : public std::ifstream
{
public:
    // constructor/destructor
    inline FileReader() : std::ifstream() {}
    inline FileReader( const char* filename ) : std::ifstream( filename ) {}
    inline ~FileReader() {}

    // stream management
    // open the stream
    void Open( const char* filename ) { std::ifstream::open( filename ); }
    // close the stream
    void Close()                      { std::ifstream::close(); }                        

protected:
    // hide base open routine (we control the mode )
    void open( const char * filename, openmode mode );

private:
    // copy operations
    FileReader(const FileReader& other);
    FileReader& operator=(const FileReader& other);
};

} // namespace gfx
