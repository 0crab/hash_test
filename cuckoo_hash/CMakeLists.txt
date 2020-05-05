cmake_minimum_required(VERSION 3.15)
project(test_libcuckoo LANGUAGES C CXX)

set(CMAKE_C_COMPILER "gcc")
set(CMAKE_CXX_COMPILER "g++")
set(CMAKE_C_STANDARD 99)
set(CMAKE_CXX_STANDARD 11)

# Add the libcuckoo interface target
add_subdirectory(libcuckoo)

# Add C interface target
add_subdirectory(libcuckoo-c)

add_library(libcuckoo_hash_table STATIC libcuckoo_hash_table.cc)
target_link_libraries(libcuckoo_hash_table libcuckoo)


add_executable(cuckoohash cuckoo_test.c cuckoo.c)
target_link_libraries(cuckoohash libcuckoo_hash_table pthread)


