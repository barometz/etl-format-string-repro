/*
This is a reproducer for an MSVC (14.44.35207 and 14.51.36231) bug that I
encountered while using ETL 20.49.0. This version of the reproducer does not
depend on ETL, demonstrating that it is a compiler bug.

It only occurs on C++20 or higher which suggests it's to do with the consteval
constructor, and is mitigated by enabling string pooling (/GF). Since string
pooling is included in the usual optimization settings it only affects a debug
build.

Expected output:

    ::basic_string_view("x").size(): 1
    ::basic_format_string("x")._size: 1
    ::basic_format_string("x")._sv._size: 1
    ::basic_format_string("x")._sv.size(): 1
    (const void*)x._sv._begin << "  " << (const void*)x._sv._end: 00B19D4C  00B19D4D

Actual output (the exact error can vary, as noted):

    ::basic_string_view("x").size(): 1
    ::basic_format_string("x")._size: 1
    ::basic_format_string("x")._sv._size: 1
    ::basic_format_string("x")._sv.size(): 5
    (const void*)x._sv._begin << "  " << (const void*)x._sv._end: 00D59C70  00D59C75
*/

#include <iostream>

class basic_string_view
{
public:
  constexpr basic_string_view(const char* begin)
      : _begin(begin)
      , _end(begin + std::char_traits<char>::length(begin))
      , _size(_end - _begin)
  {
  }

  constexpr size_t size() 
  { 
    // returning _size here gets a correct result
    return static_cast<size_t>(_end - _begin); 
  }
  
  const char* _begin;
  const char* _end;
  size_t _size;
};

struct basic_format_string
{
  // Removing this ctor and initializing with {"x"} prevents the issue.
  // Making this ctor not consteval prevents the issue.
  consteval basic_format_string(const char* fmt) : _sv(fmt), _size(_sv.size()) { }
  ::basic_string_view _sv;
  size_t _size;
};

#define PRINT_EXPR(x) std::cout << #x << ": " << x << std::endl;

int main(int /*argc*/, char* /*argv*/[])
{
  PRINT_EXPR(::basic_string_view("x").size());        // correct
  PRINT_EXPR(::basic_format_string("x")._size);       // correct
  PRINT_EXPR(::basic_format_string("x")._sv._size);   // correct
  PRINT_EXPR(::basic_format_string("x")._sv.size());  // wrong

  ::basic_format_string x("x");
  PRINT_EXPR((const void*)x._sv._begin << "  " << (const void*)x._sv._end);  // reflects the wrong size, e.g. ..8C and ..91
}
