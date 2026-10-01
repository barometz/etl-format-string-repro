/*
This is a reproducer for a bug in ETL 20.49 + MSVC 14.51.36231 where
etl::format_string::_sv (an etl::string_view) ends up with garbage extra bytes.
Sometimes it's another copy of the input string (separated from the original by
\0), but I've also seen C:\Program Files in there.

It only occurs on C++20 or higher which suggests it's to do with the consteval
constructor, and is resolved by enabling string pooling (/GF) which suggests a
compiler bug.

Expected output:

    etl::string_view("x"): x
    etl::string_view("x").length(): 1
    etl::format_string<>("x").get(): x
    etl::format_string<>("x").get().length(): 1

Actual output (the exact error can vary, as noted):

    etl::string_view("x"): x
    etl::string_view("x").length(): 1
    etl::format_string<>("x").get(): xx
    etl::format_string<>("x").get().length(): 5
*/

#include <etl/format.h>

#include <iostream>

#define PRINT_EXPR(x) std::cout << #x << ": " << x << std::endl;

int main(int /*argc*/, char* /*argv*/[])
{
  PRINT_EXPR(etl::string_view("x"));
  PRINT_EXPR(etl::string_view("x").length());
  PRINT_EXPR(etl::format_string<>("x").get());
  PRINT_EXPR(etl::format_string<>("x").get().length());
}
