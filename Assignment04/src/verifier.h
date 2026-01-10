#ifndef VERIFIER_H
#define VERIFIER_H

#include <cstdint>
#include <span>
#include <string_view>
#include <vector>

#include "bytefile.h"
#include "runtime_interface.h"

namespace assignment_04 {

    struct from_initial {};
    struct from_processed {};

    [[maybe_unused]] constexpr static from_initial from_initial_t;
    [[maybe_unused]] constexpr static from_processed from_processed_t;

    class stack_size {
    public:
        stack_size() noexcept;

        explicit stack_size(int32_t repr) noexcept;

        stack_size(from_initial, uint32_t val) noexcept;

        stack_size(from_processed, uint32_t val) noexcept;

        [[nodiscard]] int32_t get_repr() const noexcept;

        [[nodiscard]] bool has_value() const noexcept;

        [[nodiscard]] bool is_initial() const noexcept;

        [[nodiscard]] bool is_processed() const noexcept;

        [[nodiscard]] uint32_t get_value() const noexcept;

    private:
        constexpr static int32_t NO_VALUE = -1;
        int32_t repr_;
    };

    struct from_instruction {};
    struct from_frame {};

    [[maybe_unused]] constexpr static from_instruction from_instruction_t;
    [[maybe_unused]] constexpr static from_frame from_frame_t;

    class workset_entry {
    public:
        explicit workset_entry(auint repr) noexcept;

        workset_entry(from_instruction, uint32_t val) noexcept;

        workset_entry(from_frame, uint32_t val) noexcept;

        [[nodiscard]] auint get_repr() const noexcept;

        [[nodiscard]] bool is_instruction() const noexcept;

        [[nodiscard]] bool is_frame() const noexcept;

        [[nodiscard]] uint32_t get_value() const noexcept;

    private:
        auint repr_;
    };

    class verifier {
    public:
        verifier(bytefile& file, public_symbol entrypoint);

        void traverse_bytecode();

    private:
        uint32_t addr_;
        size_t frames_depth_;
        uint32_t current_frame_addr_;
        uint32_t current_frame_stack_size_;
        std::vector<stack_size> stack_sizes_;
        int32_t current_stack_size_;
        std::span<auint> workset_;
        bytefile& bytefile_;
        public_symbol entrypoint_;

        [[nodiscard]] bytecode peek_current_op() const;

        [[nodiscard]] bytecode peek_next_op() const;

        [[nodiscard]] bytecode pop_next_op();

        [[nodiscard]] varspec pop_next_varspec();

        [[nodiscard]] int32_t pop_next_int32();

        workset_entry peek() const;

        void push(workset_entry val);

        workset_entry pop();

        [[nodiscard]] int32_t get_stack_delta() const;

        void visit_op();

        void process_single_arg();

        void process_string();

        void process_sexp();

        void process_jmp();

        void process_ret();

        void process_global();

        void process_local();

        void process_argument();

        void process_cjmp();

        void process_begin();

        void process_closure();

        void process_callc();

        void process_call();

        void process_tag();

        void process_fail();

        void process_call_barray();

        [[nodiscard]] uint32_t get_globals_size() const noexcept;

        [[nodiscard]] uint32_t get_locals_size() const;

        void set_locals_size(uint32_t locals_size);

        [[nodiscard]] uint32_t get_args_size() const;

        void validate(bool condition, std::string_view message) const noexcept;
    };

    void verify(bytefile& file);

    inline int32_t stack_size::get_repr() const noexcept {
        return repr_;
    }

    inline bool stack_size::has_value() const noexcept {
        return repr_ != NO_VALUE;
    }

    inline bool stack_size::is_initial() const noexcept {
        return repr_ < NO_VALUE;
    }

    inline bool stack_size::is_processed() const noexcept {
        return repr_ > NO_VALUE;
    }

    inline uint32_t stack_size::get_value() const noexcept {
        return repr_ < NO_VALUE ? -1 * repr_ - 2 : repr_;
    }

    inline auint workset_entry::get_repr() const noexcept {
        return repr_;
    }

    inline bool workset_entry::is_instruction() const noexcept {
        return (repr_ & 1) == 0;
    }

    inline bool workset_entry::is_frame() const noexcept {
        return repr_ & 1;
    }

    inline uint32_t workset_entry::get_value() const noexcept {
        return repr_ >> 1;
    }

    inline uint32_t verifier::get_globals_size() const noexcept {
        return bytefile_.get_global_area_size();
    }

}

#endif
