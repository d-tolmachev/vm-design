#ifndef IDIOM_ANALYZER_H
#define IDIOM_ANALYZER_H

#include <cstdint>
#include <stdexcept>
#include <string>
#include <unordered_map>
#include <utility>
#include <vector>

#include "bytefile.h"

namespace assignment_03 {

    class error : public std::exception {
    public:
        error(const std::string& message, uint32_t bytecode_offset);

        [[nodiscard]] const char* what() const noexcept override;

    private:
        std::string message_;
    };

    class idiom {
    public:
        idiom(uint32_t pos, uint32_t size);

        [[nodiscard]] uint32_t get_pos() const noexcept;

        [[nodiscard]] uint32_t get_size() const noexcept;

    private:
        uint32_t pos_;
        uint32_t size_;
    };

    class idiom_hasher {
    public:
        explicit idiom_hasher(const bytefile& file) noexcept;

        size_t operator()(idiom value) const noexcept;

    private:
        const bytefile& bytefile_;
    };

    class idiom_equal {
    public:
        explicit idiom_equal(const bytefile& file) noexcept;

        bool operator()(idiom lhs, idiom rhs) const noexcept;

    private:
        const bytefile& bytefile_;
    };

    class idiom_comparator {
    public:
        explicit idiom_comparator(const bytefile& file) noexcept;

        bool operator()(idiom lhs, idiom rhs) const noexcept;

    private:
        const bytefile& bytefile_;
    };

    class idiom_printer {
    public:
        explicit idiom_printer(const bytefile& file) noexcept;

        void operator()(idiom value) const noexcept;

    private:
        const bytefile& bytefile_;
    };

    class idiom_processor {
    public:
        explicit idiom_processor(const bytefile& file);

        void find_reachable_instructions();

        void find_idioms();

        std::vector<std::pair<size_t, idiom>> sort_idioms() const;

    private:
        std::vector<bool> reachable_;
        std::vector<bool> jump_targets_;
        idiom_hasher idiom_hasher_;
        idiom_equal idiom_equal_;
        std::unordered_map<idiom, size_t, idiom_hasher, idiom_equal> idioms_frequency_;
        const bytefile& bytefile_;

        [[nodiscard]] bytecode get_op(uint32_t pos) const;

        [[nodiscard]] int32_t get_int32(uint32_t pos) const;

        static bool is_jump(bytecode op) noexcept;

        static bool is_call(bytecode op) noexcept;

        static bool is_terminal(bytecode op) noexcept;

        static void validate(bool condition, const std::string& message, uint32_t bytecode_offset);
    };

    void analyze_idioms(const bytefile& file);

    uint32_t inline idiom::get_pos() const noexcept {
        return pos_;
    }

    inline uint32_t idiom::get_size() const noexcept {
        return size_;
    }

    inline bool idiom_processor::is_jump(bytecode op) noexcept {
        return op == bytecode::JMP || op == bytecode::CJMPZ || op == bytecode::CJMPNZ;
    }

    inline bool idiom_processor::is_call(bytecode op) noexcept {
        return op == bytecode::CALLC || op == bytecode::CALL;
    }

    inline bool idiom_processor::is_terminal(bytecode op) noexcept {
        return op == bytecode::JMP || op == bytecode::END || op == bytecode::RET || op == bytecode::FAIL || op == bytecode::STOP;
    }

}

#endif
