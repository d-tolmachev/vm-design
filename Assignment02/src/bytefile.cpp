#include "bytefile.h"

namespace assignment_02 {

    public_symbol::public_symbol(size_t offset, uint32_t address, uint32_t name)
        : offset_(offset)
        , address_(address)
        , name_(name) {
    }

    size_t public_symbol::get_offset() const {
        return offset_;
    }

    uint32_t public_symbol::get_address() const {
        return address_;
    }

    uint32_t public_symbol::get_name() const {
        return name_;
    }

    bytefile::bytefile(std::string_view name)
        : name_(name)
        , global_area_size_(0)
        , code_pos_(0) {
    }

    std::string_view bytefile::get_name() const noexcept {
        return name_;
    }

    uint32_t bytefile::get_global_area_size() const noexcept {
        return global_area_size_;
    }

    void bytefile::set_global_area_size(uint32_t global_area_size) noexcept {
        global_area_size_ = global_area_size;
    }

    uint32_t bytefile::get_public_symbols_size() const noexcept {
        return public_symbols_.size();
    }

    public_symbol bytefile::get_public_symbol(uint32_t pos) const {
        return public_symbols_.at(pos);
    }

    void bytefile::add_public_symbol(public_symbol symbol) {
        public_symbols_.push_back(symbol);
    }

    uint32_t bytefile::get_string_tab_size() const noexcept {
        return string_tab_.size();
    }

    std::string_view bytefile::get_string(uint32_t pos) const {
        return {&string_tab_.at(pos)};
    }

    void bytefile::add_string(const std::vector<char>& string) {
        string_tab_.insert(string_tab_.end(), string.begin(), string.end());
    }

    std::string_view bytefile::get_public_symbol_name(uint32_t pos) const {
        return get_string(public_symbols_.at(pos).get_name());
    }

    uint32_t bytefile::get_code_pos() const noexcept {
        return code_pos_;
    }

    void bytefile::set_code_pos(uint32_t code_pos) noexcept {
        code_pos_ = code_pos;
    }

    uint32_t bytefile::get_code_size() const noexcept {
        return code_.size();
    }

    bytecode bytefile::get_code(uint32_t pos) const {
        return code_.at(pos);
    }

    void bytefile::add_code(const std::vector<bytecode>& code) {
        code_.insert(code_.end(), code.begin(), code.end());
    }

    int8_t bytefile::get_int8(uint32_t pos) const {
        return static_cast<int8_t>(code_.at(pos));
    }

    int32_t bytefile::get_int32(uint32_t pos) const {
        return *static_cast<const int32_t*>(static_cast<const void*>(&code_.at(pos)));
    }

}
