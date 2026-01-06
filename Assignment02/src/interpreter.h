#ifndef INTERPRETER_H
#define INTERPRETER_H

#include <cstdint>
#include <span>
#include <string_view>

#include "bytefile.h"
#include "stack.h"

namespace assignment_02 {

    struct from_repr {
    };

    [[maybe_unused]] constexpr static from_repr from_repr_t;

    class value;

    class aggregate {
    public:
        aggregate(from_repr, auint repr) noexcept;

        [[nodiscard]] auint get_repr() const noexcept;

        [[nodiscard]] uint32_t get_elements_size() const noexcept;

        value get_element(uint32_t pos) const;

        void set_element(uint32_t pos, value element);

    private:
        auint repr_;
    };

    class array : public aggregate {
    public:
        array(from_repr, auint repr) noexcept;

        explicit array(std::span<auint> elements);

    private:
        static auint to_array(std::span<auint> elements);
    };

    class s_expr : public aggregate {
    public:
        s_expr(from_repr, auint repr) noexcept;

        s_expr(std::string_view tag, std::span<auint> elements);

        [[nodiscard]] std::string_view get_tag() const noexcept;

    private:
        static auint to_s_expr(std::string_view tag, std::span<auint> elements);
    };

    class closure {
    public:
        closure(from_repr, auint repr) noexcept;

        explicit closure(std::span<auint> captured);

        [[nodiscard]] auint get_repr() const noexcept;

        [[nodiscard]] uint32_t get_code_offset() const noexcept;

        [[nodiscard]] uint32_t get_captured_size() const noexcept;

        value get_capture(uint32_t pos) const;

        value get_capture_reference(uint32_t pos) const;

        void set_capture(uint32_t pos, value capture);

    private:
        auint repr_;

        static auint to_closure(std::span<auint> captured);
    };

    class value {
    public:
        value() noexcept;

        value(from_repr, auint repr) noexcept;

        value(int32_t val) noexcept;

        value(const auint* val) noexcept;

        value(aggregate val) noexcept;

        value(std::string_view val);

        value(array val) noexcept;

        value(s_expr val) noexcept;

        value(closure val) noexcept;

        [[nodiscard]] auint get_repr() const noexcept;

        [[nodiscard]] bool is_empty() const noexcept;

        [[nodiscard]] bool is_integer() const noexcept;

        [[nodiscard]] bool is_reference() const noexcept;

        [[nodiscard]] bool is_aggregate() const noexcept;

        [[nodiscard]] bool is_string() const noexcept;

        [[nodiscard]] bool is_array() const noexcept;

        [[nodiscard]] bool is_s_expr() const noexcept;

        [[nodiscard]] bool is_closure() const noexcept;

        [[nodiscard]] int32_t as_integer() const noexcept;

        [[nodiscard]] auint* as_reference() const noexcept;

        [[nodiscard]] aggregate as_aggregate() const noexcept;

        [[nodiscard]] std::string_view as_string() const noexcept;

        [[nodiscard]] array as_array() const noexcept;

        [[nodiscard]] s_expr as_s_expr() const noexcept;

        [[nodiscard]] closure as_closure() const noexcept;

        [[nodiscard]] value get_string() const noexcept;

    private:
        auint repr_;

        static auint to_string(std::string_view val);
    };

    class frame {
    public:
        frame() noexcept;

        frame(stack<auint> stack, uint32_t base, uint32_t locals_size, uint32_t args_size, bool is_frame_closure = false) noexcept;

        [[nodiscard]] uint32_t get_base() const noexcept;

        [[nodiscard]] uint32_t get_locals_size() const noexcept;

        value get_local(uint32_t pos) const;

        value get_local_reference(uint32_t pos) const;

        void set_local(uint32_t pos, value local);

        [[nodiscard]] uint32_t get_args_size() const noexcept;

        value get_arg(uint32_t pos) const;

        value get_arg_reference(uint32_t pos) const;

        void set_arg(uint32_t pos, value arg);

        [[nodiscard]] bool is_closure() const noexcept;

        void is_closure(bool is_frame_closure) noexcept;

        [[nodiscard]] uint32_t get_captured_vars_size() const noexcept;

        value get_captured_var(uint32_t pos) const;

        value get_captured_var_reference(uint32_t pos) const;

        void set_captured_var(uint32_t pos, value captured_var);

        [[nodiscard]] uint32_t get_return_address() const noexcept;

        void set_return_address(uint32_t return_address) noexcept;

    private:
        stack<auint> stack_;
        uint32_t base_;
        uint32_t locals_size_;
        uint32_t args_size_;
        bool is_frame_closure_;
        uint32_t return_address_;
    };

    class state {
    public:
        explicit state(const bytefile& file) noexcept;

        ~state();

        [[nodiscard]] bytecode pop_next_op();

        void execute_binop_high();

        void execute_low_eq();

        void execute_const();

        void execute_string();

        void execute_sexp();

        void execute_sti();

        void execute_sta();

        void execute_jmp();

        bool execute_ret();

        void execute_drop();

        void execute_dup();

        void execute_swap();

        void execute_elem();

        void execute_ld_global();

        void execute_ld_local();

        void execute_ld_argument();

        void execute_ld_captured();

        void execute_lda_global();

        void execute_lda_local();

        void execute_lda_argument();

        void execute_lda_captured();

        void execute_st_global();

        void execute_st_local();

        void execute_st_argument();

        void execute_st_captured();

        void execute_cjmp(bool nz);

        void execute_begin();

        void execute_cbegin();

        void execute_closure();

        void execute_callc();

        void execute_call();

        void execute_tag();

        void execute_array();

        void execute_fail();

        void execute_line();

        void execute_patt_str();

        void execute_patt_string();

        void execute_patt_array();

        void execute_patt_sexp();

        void execute_patt_ref();

        void execute_patt_val();

        void execute_patt_fun();

        void execute_call_lread();

        void execute_call_lwrite();

        void execute_call_llength();

        void execute_call_lstring();

        void execute_call_barray();

        void validate(bool condition, std::string_view message) const noexcept;

    private:
        uint32_t ip_;
        std::span<frame> frames_;
        stack<auint> stack_;
        bool is_tmp_closure_;
        const bytefile& bytefile_;

        [[nodiscard]] bytecode peek_current_op() const;

        [[nodiscard]] bytecode peek_next_op() const;

        [[nodiscard]] int8_t pop_next_int8();

        [[nodiscard]] int32_t pop_next_int32();

        [[nodiscard]] bool has_frame() const noexcept;

        const frame& peek_frame() const;

        frame& peek_frame();

        void push_frame(const frame& frame);

        frame pop_frame();

        value peek(uint32_t offset = 0) const;

        void push(value val);

        value pop();

        void pop(uint32_t count);

        [[nodiscard]] uint32_t get_globals_size() const noexcept;

        value get_global(uint32_t pos) const;

        value get_global_reference(uint32_t pos) const;

        void set_global(uint32_t pos, value global);
    };

    void interpret(const bytefile& file);

    inline auint aggregate::get_repr() const noexcept {
        return repr_;
    }

    inline std::string_view s_expr::get_tag() const noexcept {
        return {reinterpret_cast<char*>(TO_SEXP(reinterpret_cast<void*>(get_repr()))->tag)};
    }

    inline auint closure::get_repr() const noexcept {
        return repr_;
    }

    inline uint32_t closure::get_code_offset() const noexcept {
        return UNBOX(reinterpret_cast<auint*>(TO_DATA(reinterpret_cast<void*>(repr_))->contents)[0]);
    }

    inline auint value::get_repr() const noexcept {
        return repr_;
    }

    inline bool value::is_empty() const noexcept {
        return repr_ == 0;
    }

    inline bool value::is_integer() const noexcept {
        return (repr_ & 1) != 0;
    }

    inline bool value::is_reference() const noexcept {
        return (repr_ & 1) == 0;
    }

    inline int32_t value::as_integer() const noexcept {
        return UNBOX(repr_);
    }

    inline auint* value::as_reference() const noexcept {
        return reinterpret_cast<auint*>(repr_);
    }

    inline aggregate value::as_aggregate() const noexcept {
        return aggregate{from_repr_t, repr_};
    }

    inline std::string_view value::as_string() const noexcept {
        return std::string_view{TO_DATA(reinterpret_cast<void*>(repr_))->contents};
    }

    inline array value::as_array() const noexcept {
        return array{from_repr_t, repr_};
    }

    inline s_expr value::as_s_expr() const noexcept {
        return s_expr{from_repr_t, repr_};
    }

    inline closure value::as_closure() const noexcept {
        return closure{from_repr_t, repr_};
    }

    inline uint32_t frame::get_base() const noexcept {
        return base_;
    }

    inline uint32_t frame::get_locals_size() const noexcept {
        return locals_size_;
    }

    inline uint32_t frame::get_args_size() const noexcept {
        return args_size_;
    }

    inline bool frame::is_closure() const noexcept {
        return is_frame_closure_;
    }

    inline void frame::is_closure(bool is_frame_closure) noexcept {
        is_frame_closure_ = is_frame_closure;
    }

    inline uint32_t frame::get_return_address() const noexcept {
        return return_address_;
    }

    inline void frame::set_return_address(uint32_t return_address) noexcept {
        return_address_ = return_address;
    }

    inline bool state::has_frame() const noexcept {
        return !frames_.empty();
    }

    inline uint32_t state::get_globals_size() const noexcept {
        return bytefile_.get_global_area_size();
    }

}

#endif
