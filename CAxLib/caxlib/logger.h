#ifndef CAX_LOGGER_H
#define CAX_LOGGER_H

#include <iostream>
#include <fstream>
#include <ostream>
#include <streambuf>

namespace caxlib
{

// http://stackoverflow.com/questions/772355/how-to-inherit-from-stdostream
//
struct Logger : std::ostream, std::streambuf
{
    Logger();

    int  overflow(int c);
    void dispatch_cout(char c);
    void dispatch_file(char c);
    void enable();
    void disable();
    void set_log_file(const char *filename);

    bool dispatch_to_cout;
    bool dispatch_to_file;
    std::ofstream f;
};

CAX_INLINE std::ostream & newl (std::ostream & os);
CAX_INLINE std::ostream & endl (std::ostream & os);
CAX_INLINE std::ostream & flush(std::ostream & os);

}

#ifndef  CAX_STATIC_LIB
#include "logger.cpp"
#endif

#endif // CAX_LOGGER_H
