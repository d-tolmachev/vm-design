cmake_minimum_required(VERSION 3.8)

project(Assignment04)

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

add_executable(Assignment04
        src/bytefile.cpp
        src/file_reader.cpp
        src/interpreter.cpp
        src/main.cpp
        src/verifier.h
        src/verifier.cpp
)

target_include_directories(Assignment04 PRIVATE ${PROJECT_SOURCE_DIR}/../runtime)
target_link_libraries(Assignment04 PRIVATE runtime)
target_link_options(Assignment04 PRIVATE "LINKER:--defsym=__start_custom_data=0" "LINKER:--defsym=__stop_custom_data=0")
