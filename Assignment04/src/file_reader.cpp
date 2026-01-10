#include "file_reader.h"

#include <cstdint>
#include <filesystem>
#include <fstream>
#include <stdexcept>
#include <vector>

namespace assignment_04 {

    bytefile read_file(std::string_view path) {
        if (!std::filesystem::exists(path)) {
            throw std::runtime_error("File not found");
        }
        std::ifstream is(path.data(), std::ios::binary);
        size_t pos = 0;
        uint32_t string_tab_size = 0;
        is.read(static_cast<char*>(static_cast<void*>(&string_tab_size)), sizeof(uint32_t));
        pos += is.gcount();
        if (is.gcount() < sizeof(uint32_t)) {
            throw std::runtime_error("Unexpected end of file");
        }
        uint32_t global_area_size = 0;
        is.read(static_cast<char*>(static_cast<void*>(&global_area_size)), sizeof(uint32_t));
        pos += is.gcount();
        if (is.gcount() < sizeof(uint32_t)) {
            throw std::runtime_error("Unexpected end of file");
        }
        int32_t public_symbols_size = 0;
        is.read(static_cast<char*>(static_cast<void*>(&public_symbols_size)), sizeof(int32_t));
        pos += is.gcount();
        if (is.gcount() < sizeof(int32_t)) {
            throw std::runtime_error("Unexpected end of file");
        }
        if (public_symbols_size <= 0) {
            throw std::runtime_error("Invalid symbol table size");
        }
        is.seekg(0, std::ios::end);
        int64_t code_size = is.tellg();
        if (code_size == -1) {
            throw std::runtime_error("Failed while determining filesize");
        }
        is.seekg(static_cast<int64_t>(pos), std::ios::beg);
        code_size -= static_cast<int64_t>(pos + 2 * public_symbols_size * sizeof(uint32_t) + string_tab_size * sizeof(char));
        bytefile file(path, string_tab_size, public_symbols_size, code_size);
        file.set_global_area_size(global_area_size);
        for (uint32_t i = 0; i < public_symbols_size; ++i) {
            size_t offset = pos;
            uint32_t name = 0;
            is.read(static_cast<char*>(static_cast<void*>(&name)), sizeof(uint32_t));
            pos += is.gcount();
            if (is.gcount() < sizeof(uint32_t)) {
                throw std::runtime_error("Unexpected end of file");
            }
            uint32_t address = 0;
            is.read(static_cast<char*>(static_cast<void*>(&address)), sizeof(uint32_t));
            pos += is.gcount();
            if (is.gcount() < sizeof(uint32_t)) {
                throw std::runtime_error("Unexpected end of file");
            }
            file.add_public_symbol(public_symbol{offset, name, address,});
        }
        std::vector<char> string_tab(string_tab_size);
        is.read(string_tab.data(), static_cast<int64_t>(string_tab.size()));
        pos += is.gcount();
        if (is.gcount() < string_tab.size()) {
            throw std::runtime_error("Unexpected end of file");
        }
        file.add_string(string_tab);
        file.set_code_pos(pos);
        std::vector<bytecode> code(code_size);
        is.read(static_cast<char*>(static_cast<void*>(code.data())), static_cast<int64_t>(code.size()));
        pos += is.gcount();
        if (is.gcount() < code.size()) {
            throw std::runtime_error("Unexpected end of file");
        }
        file.add_code(code);
        return file;
    }

}
