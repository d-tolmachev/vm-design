#ifndef FILE_READER_H
#define FILE_READER_H

#include <filesystem>

#include "bytefile.h"

namespace assignment_02 {

    bytefile read_file(const std::filesystem::path& path);

}

#endif
