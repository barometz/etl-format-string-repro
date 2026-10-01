cmake_minimum_required(VERSION 4.0)

project(etl-fmt-string-repro)

# Enables string pooling, uncomment to fix:
# add_compile_options(/GF)

# Only occurs on C++20 or newer
add_compile_options(/std:c++20)

add_executable(${PROJECT_NAME} main.cpp)
