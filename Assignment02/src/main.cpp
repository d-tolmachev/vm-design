#include <iostream>
#include <stdexcept>

#include "bytefile.h"
#include "file_reader.h"
#include "interpreter.h"

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <filename>" << std::endl;
        return -1;
    }
    try {
        assignment_02::bytefile file = assignment_02::read_file(argv[1]);
        assignment_02::interpret(file);
    } catch (const std::exception& exc) {
        std::cerr << exc.what() << std::endl;
        return -1;
    }
    return 0;
}
