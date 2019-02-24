#pragma once

#include <stdexcept>
#include <string>

namespace tell
{
  namespace impl 
  {
    struct Error : std::runtime_error
    {
      Error(const std::string& s, const char* file, int line);
    };
    
    void report_failure(const char* cond, const char* file, int line);  
  }
}

#define xassert(cond)							\
  ((cond) ? (void) 0 : tell::impl::report_failure(#cond, __FILE__, __LINE__));
  
inline
tell::impl::Error::Error(const std::string& s, const char* file, int line) 
  : std::runtime_error(
		  "assertion failed: " + s 
		  + ", file " + file 
		  + ", line " + lexical_cast<std::string>(line)) 
{
  // breakpoint
}

inline
void tell::impl::report_failure(const char* cond, const char* file, int line)
{
  throw Error(cond, file, line);
}
