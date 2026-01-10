#include "bytefile.h"

#include <algorithm>

namespace assignment_04 {

    public_symbol::public_symbol(size_t offset, uint32_t name, uint32_t address) noexcept
        : offset_(offset)
        , name_(name)
        , address_(address) {
    }

    bytefile::bytefile(std::string_view name, uint32_t string_tab_size, uint32_t public_symbols_size, uint32_t code_size)
        : base_(static_cast<::bytefile*>(static_cast<void*>(new (std::align_val_t{alignof(::bytefile)}) std::byte[4 * sizeof(char*) + 3 * sizeof(int) + 2 * public_symbols_size * sizeof(int) + string_tab_size * sizeof(char) + code_size * sizeof(char)])))
        , name_(name)
        , code_pos_(0)
        , code_size_(0) {
        base_->string_ptr = base_->buffer + 2 * public_symbols_size * sizeof(int);
        base_->public_ptr = static_cast<int*>(static_cast<void*>(base_->buffer));
        base_->code_ptr = base_->buffer + 2 * public_symbols_size * sizeof(int) + string_tab_size * sizeof(char);
    }

    bytefile::~bytefile() {
        delete[] base_;
    }

    public_symbol bytefile::get_public_symbol(uint32_t pos) const {
        return public_symbol{offsets_.at(pos), static_cast<uint32_t>(base_->public_ptr[2 * pos]), static_cast<uint32_t>(base_->public_ptr[2 * pos + 1])};
    }

    void bytefile::add_public_symbol(public_symbol symbol) {
        offsets_.push_back(symbol.get_offset());
        base_->public_ptr[2 * base_->public_symbols_number] = static_cast<int>(symbol.get_name());
        base_->public_ptr[2 * base_->public_symbols_number + 1] = static_cast<int>(symbol.get_address());
        ++base_->public_symbols_number;
    }

    std::string_view bytefile::get_string(uint32_t pos) const {
        return std::string_view{&base_->string_ptr[pos]};
    }

    void bytefile::add_string(const std::vector<char>& string) {
        std::copy(string.begin(), string.end(), base_->string_ptr + base_->stringtab_size * sizeof(char));
        base_->stringtab_size += static_cast<int>(string.size());
    }

    std::string_view bytefile::get_public_symbol_name(uint32_t pos) const {
        return get_string(base_->public_ptr[2 * pos]);
    }

    bytecode bytefile::get_code(uint32_t pos) const {
        return static_cast<bytecode>(base_->code_ptr[pos]);
    }

    void bytefile::set_code(uint32_t pos, bytecode code) {
        base_->code_ptr[pos] = static_cast<char>(code);
    }

    void bytefile::add_code(const std::vector<bytecode>& code) {
        std::copy(code.begin(), code.end(), static_cast<bytecode*>(static_cast<void*>(base_->code_ptr)) + code_size_ * sizeof(bytecode));
        code_size_ += code.size();
    }

    varspec bytefile::get_varspec(uint32_t pos) const {
        return static_cast<varspec>(base_->code_ptr[pos]);
    }

    void bytefile::set_varspec(uint32_t pos, varspec spec) {
        base_->code_ptr[pos] = static_cast<char>(spec);
    }

    int32_t bytefile::get_int32(uint32_t pos) const {
        return *static_cast<const int32_t*>(static_cast<const void*>(&base_->code_ptr[pos]));
    }

    void bytefile::set_int32(uint32_t pos, int32_t int32) {
        *static_cast<int32_t*>(static_cast<void*>(&base_->code_ptr[pos])) = int32;
    }

    std::span<const bytecode> bytefile::get_bytes(uint32_t pos, uint32_t count) const {
        return std::span<const bytecode>{static_cast<const bytecode*>(static_cast<const void*>(&base_->code_ptr[pos])), count};
    }

}
