#include "idiom_analyzer.h"

#include <algorithm>
#include <cstdio>
#include <iomanip>
#include <iostream>
#include <span>
#include <sstream>

#include "byterun_interface.h"

namespace assignment_03 {

    error::error(const std::string& message, uint32_t bytecode_offset) {
        std::ostringstream os;
        os << message << ". Bytecode offset: " << std::hex << std::showbase << bytecode_offset;
        message_ = os.str();
    }

    const char* error::what() const noexcept {
        return message_.c_str();
    }

    idiom::idiom(uint32_t pos, uint32_t size)
        : pos_(pos)
        , size_(size) {
    }

    idiom_equal::idiom_equal(const bytefile& file) noexcept
        : bytefile_(file) {
    }

    bool idiom_equal::operator()(idiom lhs, idiom rhs) const noexcept {
        if (lhs.get_size() != rhs.get_size()) {
            return false;
        }
        uint32_t length = 0;
        for (uint32_t i = 0; i < lhs.get_size(); ++i) {
            length += disassemble_instruction(stdin, bytefile_.get_base(), static_cast<int>(lhs.get_pos() + length));
        }
        std::span<const bytecode> lhs_bytes = bytefile_.get_bytes(lhs.get_pos(), length);
        length = 0;
        for (uint32_t i = 0; i < rhs.get_size(); ++i) {
            length += disassemble_instruction(stdin, bytefile_.get_base(), static_cast<int>(rhs.get_pos() + length));
        }
        std::span<const bytecode> rhs_bytes = bytefile_.get_bytes(rhs.get_pos(), length);
        return std::equal(lhs_bytes.begin(), lhs_bytes.end(), rhs_bytes.begin(), rhs_bytes.end());
    }

    idiom_comparator::idiom_comparator(const bytefile& file) noexcept
        : bytefile_(file) {
    }

    bool idiom_comparator::operator()(idiom lhs, idiom rhs) const noexcept {
        if (lhs.get_size() < rhs.get_size()) {
            return true;
        }
        if (lhs.get_size() > rhs.get_size()) {
            return false;
        }
        uint32_t length = 0;
        for (uint32_t i = 0; i < lhs.get_size(); ++i) {
            length += disassemble_instruction(stdin, bytefile_.get_base(), static_cast<int>(lhs.get_pos() + length));
        }
        std::span<const bytecode> lhs_bytes = bytefile_.get_bytes(lhs.get_pos(), length);
        length = 0;
        for (uint32_t i = 0; i < rhs.get_size(); ++i) {
            length += disassemble_instruction(stdin, bytefile_.get_base(), static_cast<int>(rhs.get_pos() + length));
        }
        std::span<const bytecode> rhs_bytes = bytefile_.get_bytes(rhs.get_pos(), length);
        return std::lexicographical_compare(lhs_bytes.begin(), lhs_bytes.end(), rhs_bytes.begin(), rhs_bytes.end());
    }

    idiom_printer::idiom_printer(const bytefile& file) noexcept
        : bytefile_(file) {
    }

    void idiom_printer::operator()(idiom value) const noexcept {
        uint32_t length = 0;
        for (uint32_t i = 0; i < value.get_size(); ++i) {
            if (i > 0) {
                std::cout << "; ";
            }
            length += disassemble_instruction(stdout, bytefile_.get_base(), static_cast<int>(value.get_pos() + length));
        }
    }

    idiom_processor::idiom_processor(const bytefile& file)
        : reachable_(file.get_code_size() + 1)
        , jump_targets_(file.get_code_size())
        , bytefile_(file) {
        reachable_[reachable_.size() - 1] = true;
    }

    void idiom_processor::find_reachable_instructions() {
        std::vector<size_t> workset;
        for (uint32_t i = 0; i < bytefile_.get_public_symbols_size(); ++i) {
            public_symbol symbol = bytefile_.get_public_symbol(i);
            validate(symbol.get_address() < bytefile_.get_code_size(), "Invalid symbol offset", symbol.get_address());
            if (!reachable_.at(symbol.get_address())) {
                jump_targets_[symbol.get_address()] = true;
                reachable_[symbol.get_address()] = true;
                workset.push_back(symbol.get_address());
            }
        }
        while (!workset.empty()) {
            uint32_t addr = workset.back();
            workset.pop_back();
            bytecode op = get_op(addr);
            uint32_t length = disassemble_instruction(stdin, bytefile_.get_base(), static_cast<int>(addr));
            validate(addr + length < bytefile_.get_code_size(), "Unexpected end of code", addr + length);
            if (is_jump(op) || is_call(op)) {
                uint32_t target = get_int32(addr + sizeof(bytecode));
                validate(target < bytefile_.get_code_size(), "Invalid jump/call destination", target);
                jump_targets_[target] = true;
                if (!reachable_.at(target)) {
                    reachable_[target] = true;
                    workset.push_back(target);
                }
            }
            if (!is_terminal(op)) {
                if (is_call(op)) {
                    jump_targets_[addr + length] = true;
                }
                if (!reachable_.at(addr + length)) {
                    reachable_[addr + length] = true;
                    workset.push_back(addr + length);
                }
            }
        }
    }

    void idiom_processor::find_idioms() {
        uint32_t addr = 0;
        while (addr < bytefile_.get_code_size()) {
            while (!reachable_.at(addr)) {
                ++addr;
            }
            if (addr >= bytefile_.get_code_size()) {
                return;
            }
            bytecode op = get_op(addr);
            uint32_t length = disassemble_instruction(stdin, bytefile_.get_base(), static_cast<int>(addr));
            validate(addr + length < bytefile_.get_code_size(), "Unexpected end of code", addr + length);
            idioms_.emplace_back(addr, 1);
            if (!is_call(op) && !is_terminal(op) && reachable_.at(addr + length) && !jump_targets_.at(addr + length)) {
                uint32_t next_length = disassemble_instruction(stdin, bytefile_.get_base(), static_cast<int>(addr + length));
                validate(addr + length + next_length < bytefile_.get_code_size(), "Unexpected end of code", addr + length + next_length);
                idioms_.emplace_back(addr, 2);
            }
            addr += length;
        }
    }

    std::vector<std::pair<size_t, idiom>> idiom_processor::sort_idioms() {
        auto compare = [comparator = idiom_comparator{bytefile_}](const std::pair<size_t, idiom>& lhs, const std::pair<size_t, idiom>& rhs) {
            return lhs.first > rhs.first || (lhs.first == rhs.first && comparator(lhs.second, rhs.second));
        };
        idiom_equal equal(bytefile_);
        std::vector<std::pair<size_t, idiom>> idioms_frequency;
        std::sort(idioms_.begin(), idioms_.end(), idiom_comparator{bytefile_});
        size_t frequency = 1;
        for (size_t i = 1; i < idioms_.size(); ++i) {
            if (!equal(idioms_.at(i - 1), idioms_.at(i))) {
                idioms_frequency.emplace_back(frequency, idioms_.at(i - 1));
                frequency = 1;
            } else {
                ++frequency;
            }
        }
        idioms_frequency.emplace_back(frequency, idioms_.back());
        std::sort(idioms_frequency.begin(), idioms_frequency.end(), compare);
        return idioms_frequency;
    }

    bytecode idiom_processor::get_op(uint32_t pos) const {
        validate(pos + sizeof(bytecode) < bytefile_.get_code_size(), "Unexpected end of code", pos + sizeof(bytecode));
        return bytefile_.get_code(pos);
    }

    int32_t idiom_processor::get_int32(uint32_t pos) const {
        validate(pos + sizeof(int32_t) < bytefile_.get_code_size(), "Unexpected end of code", pos + sizeof(int32_t));
        return bytefile_.get_int32(pos);
    }

    void idiom_processor::validate(bool condition, const std::string& message, uint32_t bytecode_offset) {
        if (!condition) {
            throw error(message, bytecode_offset);
        }
    }

    void analyze_idioms(const bytefile& file) {
        idiom_printer printer(file);
        idiom_processor processor(file);
        processor.find_reachable_instructions();
        processor.find_idioms();
        std::vector<std::pair<size_t, idiom>> idioms = processor.sort_idioms();
        for (const auto& [frequency, bytes] : idioms) {
            std::cout << frequency << " ";
            printer(bytes);
            std::cout << std::endl;
        }
    }

}
