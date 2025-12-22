cmake_minimum_required(VERSION 3.8)

project(Assignment02)

set(CMAKE_CXX_STANDARD 20)

find_program(MAKE NAMES make gmake)

add_custom_command(
        OUTPUT ${PROJECT_SOURCE_DIR}/runtime/runtime.a
        COMMAND ${MAKE} -C ${PROJECT_SOURCE_DIR}/runtime
        WORKING_DIRECTORY ${PROJECT_SOURCE_DIR}/runtime
)

add_custom_target(runtime
        DEPENDS ${PROJECT_SOURCE_DIR}/runtime/runtime.a
)

add_library(runtime_lib STATIC IMPORTED)

add_dependencies(runtime_lib runtime)

set_source_files_properties(${PROJECT_SOURCE_DIR}/runtime/runtime.a
        PROPERTIES GENERATED TRUE
)
set_target_properties(runtime_lib PROPERTIES
        IMPORTED_LOCATION ${PROJECT_SOURCE_DIR}/runtime/runtime.a
)

add_executable(Assignment02
        src/bytefile.cpp
        src/file_reader.cpp
        src/interpreter.cpp
        src/main.cpp
)

target_include_directories(Assignment02 PRIVATE ${PROJECT_SOURCE_DIR}/runtime)
target_link_libraries(Assignment02 PRIVATE runtime_lib)
target_link_options(Assignment02 PRIVATE "LINKER:--defsym=__start_custom_data=0" "LINKER:--defsym=__stop_custom_data=0")
