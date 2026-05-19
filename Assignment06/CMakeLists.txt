cmake_minimum_required(VERSION 3.8)

project(Assignment06)

set(CMAKE_CXX_STANDARD 20)

add_library(Assignment06 STATIC
        src/pool.cpp
)

add_executable(test
        ../memory_pool/test.cpp
)

target_include_directories(test PRIVATE ${PROJECT_SOURCE_DIR}/src)
target_link_libraries(test PRIVATE Assignment06)
