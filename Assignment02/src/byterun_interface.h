#ifndef BYTERUN_INTERFACE_H
#define BYTERUN_INTERFACE_H

#include <cstdio>

extern "C" {
typedef struct {
    char* string_ptr;
    int* public_ptr;
    char* code_ptr;
    int* global_ptr;
    int stringtab_size;
    int global_area_size;
    int public_symbols_number;
    char buffer[0];
} bytefile;
}

#endif
