cmake_minimum_required(VERSION 3.5)
project(leveldb_1_20)

set(CMAKE_CXX_STANDARD 11)

set(CXX_FLAGS
    -Wall
    -Wextra
    -fno-builtin-memcmp
    -fPIC)


string(REPLACE ";" " " CMAKE_CXX_FLAGS "${CXX_FLAGS}") 

set(CMAKE_CXX_FLAGS_DEBUG "-O0 -g")
set(CMAKE_CXX_FLAGS_RELEASE "-O2 -DNDEBUG")

message(STATUS ${CMAKE_CXX_FLAGS})

set(SOURCE_FILES
    db/builder.cc
    db/c.cc
    db/dbformat.cc
    db/db_impl.cc
    db/db_iter.cc
    db/dumpfile.cc
    db/filename.cc
    db/log_reader.cc
    db/log_writer.cc
    db/memtable.cc
    db/repair.cc
    db/table_cache.cc
    db/version_edit.cc
    db/version_set.cc
    db/write_batch.cc
    helpers/memenv/memenv.cc
    port/port_posix.cc
    port/port_posix_sse.cc
    table/block_builder.cc
    table/block.cc
    table/filter_block.cc
    table/format.cc
    table/iterator.cc
    table/merger.cc
    table/table_builder.cc
    table/table.cc
    table/two_level_iterator.cc
    util/arena.cc
    util/bloom.cc
    util/cache.cc
    util/coding.cc
    util/comparator.cc
    util/crc32c.cc
    util/env.cc
    util/env_posix.cc
    util/filter_policy.cc
    util/hash.cc
    util/histogram.cc
    util/logging.cc
    util/options.cc
    util/status.cc)

include_directories(
    ${CMAKE_SOURCE_DIR}
    ${CMAKE_SOURCE_DIR}/include)

add_library(leveldb SHARED ${SOURCE_FILES})
set_target_properties(leveldb PROPERTIES SOVERSION 1.20)
add_definitions(-DOS_LINUX
        -DLEVELDB_PLATFORM_POSIX
        -DLEVELDB_ATOMIC_PRESENT
        -DSNAPPY)

add_executable(leveldb_test ${SOURCE_FILES} test_main.cpp)
target_link_libraries(leveldb_test snappy pthread)
