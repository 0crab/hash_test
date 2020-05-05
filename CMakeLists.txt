cmake_minimum_required(VERSION 3.15)
project(hash_test C)

set(CMAKE_C_STANDARD 99)

add_subdirectory(source_test)

add_subdirectory(cuckoo_hash)

add_executable(hash_test main.c)