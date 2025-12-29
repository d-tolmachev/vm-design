#ifndef FILE_READER_H
#define FILE_READER_H

#include <string_view>

#include "bytefile.h"

namespace assignment_02 {

    bytefile read_file(std::string_view path);

}

#endif
