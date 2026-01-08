cmake_minimum_required(VERSION 3.8)

project(Assignment03)

set(CMAKE_CXX_STANDARD 20)

find_program(MAKE NAMES make gmake)

add_custom_command(
        OUTPUT ${PROJECT_SOURCE_DIR}/../runtime/runtime.a
        COMMAND ${MAKE} -C ${PROJECT_SOURCE_DIR}/../runtime
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/../runtime
)

add_custom_target(runtime_target
        DEPENDS ${PROJECT_SOURCE_DIR}/../runtime/runtime.a
)

add_library(runtime STATIC IMPORTED)

add_dependencies(runtime runtime_target)

set_source_files_properties(${PROJECT_SOURCE_DIR}/../runtime/runtime.a
        PROPERTIES GENERATED TRUE
)
set_target_properties(runtime PROPERTIES
        IMPORTED_LOCATION ${PROJECT_SOURCE_DIR}/../runtime/runtime.a
)

add_library(byterun STATIC
        ../byterun/byterun.c
)

target_include_directories(byterun PRIVATE ${PROJECT_SOURCE_DIR}/../runtime)
target_link_libraries(byterun PRIVATE runtime)

add_executable(Assignment03
        src/bytefile.cpp
        src/file_reader.cpp
        src/idiom_analyzer.cpp
        src/main.cpp
)

target_include_directories(Assignment03 PRIVATE ${PROJECT_SOURCE_DIR}/../runtime)
target_link_libraries(Assignment03 PRIVATE byterun)
