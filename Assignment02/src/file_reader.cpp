#include "file_reader.h"

#include <cstdint>
#include <fstream>
#include <stdexcept>
#include <vector>

namespace assignment_02 {

    bytefile read_file(const std::filesystem::path& path) {
        constexpr static size_t BUF_SIZE = 4096;
        if (!std::filesystem::exists(path)) {
            throw std::runtime_error("unable to found " + path.string());
        }
        bytefile file(path.string());
        std::ifstream is(path, std::ios::binary);
        size_t pos = 0;
        uint32_t string_tab_size = 0;
        is.read(static_cast<char*>(static_cast<void*>(&string_tab_size)), sizeof(uint32_t));
        pos += is.gcount();
        uint32_t global_area_size = 0;
        is.read(static_cast<char*>(static_cast<void*>(&global_area_size)), sizeof(uint32_t));
        pos += is.gcount();
        file.set_global_area_size(global_area_size);
        uint32_t public_symbols_size = 0;
        is.read(static_cast<char*>(static_cast<void*>(&public_symbols_size)), sizeof(uint32_t));
        pos += is.gcount();
        for (uint32_t i = 0; i < public_symbols_size; ++i) {
            size_t offset = pos;
            uint32_t address = 0;
            is.read(static_cast<char*>(static_cast<void*>(&address)), sizeof(uint32_t));
            pos += is.gcount();
            uint32_t name = 0;
            is.read(static_cast<char*>(static_cast<void*>(&name)), sizeof(uint32_t));
            pos += is.gcount();
            file.add_public_symbol(public_symbol{offset, address, name});
        }
        std::vector<char> string_tab(string_tab_size);
        is.read(string_tab.data(), static_cast<int64_t>(string_tab.size()));
        pos += is.gcount();
        file.add_string(string_tab);
        file.set_code_pos(pos);
        std::vector<bytecode> code(BUF_SIZE);
        while (is) {
            is.read(static_cast<char*>(static_cast<void*>(code.data())), static_cast<int64_t>(code.size()));
            pos += is.gcount();
            if (is.gcount() < BUF_SIZE) {
                code.resize(is.gcount());
            }
            file.add_code(code);
        }
        return file;
    }

}
