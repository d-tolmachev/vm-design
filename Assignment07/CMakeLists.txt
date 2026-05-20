cmake_minimum_required(VERSION 3.8)

project(Assignment07)

set(CMAKE_CXX_STANDARD 20)

add_library(Assignment07 STATIC
        src/pool.cpp
        src/pool_registry.cpp
        src/sigsegv_handler.cpp
)

add_executable(test
        memory_pool/test.cpp
)

target_include_directories(test PRIVATE ${PROJECT_SOURCE_DIR}/src)
target_link_libraries(test PRIVATE Assignment07)
