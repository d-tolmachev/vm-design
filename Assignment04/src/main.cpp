#include <iostream>
#include <stdexcept>

#include "bytefile.h"
#include "file_reader.h"
#include "interpreter.h"
#include "verifier.h"

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <filename>" << std::endl;
        return -1;
    }
    try {
        assignment_04::bytefile file = assignment_04::read_file(argv[1]);
        assignment_04::verify(file);
        assignment_04::interpret(file);
    } catch (const std::exception& exc) {
        std::cerr << exc.what() << std::endl;
        return -1;
    }
    return 0;
}
