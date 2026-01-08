#include <iostream>
#include <stdexcept>

#include "bytefile.h"
#include "file_reader.h"
#include "idiom_analyzer.h"

int main(int argc, char** argv) {
    if (argc != 2) {
        std::cerr << "Usage: " << argv[0] << " <filename>" << std::endl;
        return -1;
    }
    try {
        assignment_03::bytefile file = assignment_03::read_file(argv[1]);
        assignment_03::analyze_idioms(file);
    } catch (const std::exception& exc) {
        std::cerr << exc.what() << std::endl;
        return -1;
    }
    return 0;
}
