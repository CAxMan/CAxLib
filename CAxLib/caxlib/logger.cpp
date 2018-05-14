#include "logger.h"
#include "caxlib.h"

namespace caxlib
{

CAX_INLINE
Logger::Logger() : std::ostream(this)
{
    enable();
}

CAX_INLINE
int Logger::overflow(int c)
{
    if (dispatch_to_cout) dispatch_cout(c);
    if (dispatch_to_file) dispatch_file(c);
    return 0;
}

CAX_INLINE
void Logger::dispatch_cout(char c)
{
    std::cout.put(c);
}

CAX_INLINE
void Logger::dispatch_file(char c)
{
    f.put(c);
}

CAX_INLINE
void Logger::enable()
{
    dispatch_to_cout = true;
    dispatch_to_file = f.is_open();
}

CAX_INLINE
void Logger::disable()
{
    dispatch_to_cout = false;
    dispatch_to_file = false;
}

CAX_INLINE
void Logger::set_log_file(const char * filename)
{
    f.open(filename);
    if (f.is_open()) dispatch_to_file = true;
}

// https://github.com/isocpp/CppCoreGuidelines/issues/357
//
CAX_INLINE
std::ostream & newl(std::ostream & os)
{
    return os << "\n";
}

CAX_INLINE
std::ostream & flush(std::ostream & os)
{
    os.flush();
    return os;
}

CAX_INLINE
std::ostream & endl(std::ostream & os)
{
    newl(os);
    flush(os);
    return os;
}

}
