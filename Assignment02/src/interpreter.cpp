#include "interpreter.h"

namespace assignment_02 {

    constexpr static size_t MAX_FRAMES_SIZE = 0xFFFF;
    constexpr static size_t MAX_STACK_SIZE = 0xFFFFF;
    static std::array<frame, MAX_FRAMES_SIZE> frames_buf;
    static std::array<auint, MAX_STACK_SIZE> stack_buf{};

    aggregate::aggregate(from_repr, auint repr) noexcept
        : repr_(repr) {
    }

    auint aggregate::get_repr() const noexcept {
        return repr_;
    }

    uint32_t aggregate::get_elements_size() const noexcept {
        return UNBOX(Llength(reinterpret_cast<void*>(repr_)));
    }

    value aggregate::get_element(uint32_t pos) const {
        return value{static_cast<auint*>(Belem(reinterpret_cast<void*>(repr_), static_cast<aint>(BOX(static_cast<int32_t>(pos)))))};
    }

    void aggregate::set_element(uint32_t pos, value element) {
        Bsta(reinterpret_cast<void*>(repr_), static_cast<aint>(BOX(static_cast<int32_t>(pos))), element.as_reference());
    }

    array::array(from_repr, auint repr) noexcept
        : aggregate(from_repr_t, repr) {
    }

    array::array(std::span<auint> elements)
        : aggregate(from_repr_t, to_array(elements)) {
    }

    auint array::to_array(std::span<auint> elements) {
        return reinterpret_cast<auint>(Barray(static_cast<aint*>(static_cast<void*>(elements.data())), BOX(elements.size())));
    }

    s_expr::s_expr(from_repr, auint repr) noexcept
        : aggregate(from_repr_t, repr) {
    }

    s_expr::s_expr(std::string_view tag, std::span<auint> elements)
        : aggregate(from_repr_t, to_s_expr(tag, elements)) {
    }

    std::string_view s_expr::get_tag() const noexcept {
        return {reinterpret_cast<char*>(TO_SEXP(reinterpret_cast<void*>(get_repr()))->tag)};
    }

    auint s_expr::to_s_expr(std::string_view tag, std::span<auint> elements) {
        elements[elements.size() - 1] = LtagHash(const_cast<char*>(tag.data()));
        return reinterpret_cast<auint>(Bsexp(static_cast<aint*>(static_cast<void*>(elements.data())), BOX(elements.size())));
    }

    closure::closure(from_repr, auint repr) noexcept
        : repr_(repr) {
    }

    closure::closure(std::span<auint> captured)
        : repr_(to_closure(captured)) {
    }

    auint closure::get_repr() const noexcept {
        return repr_;
    }

    uint32_t closure::get_code_offset() const noexcept {
        return UNBOX(reinterpret_cast<auint*>(TO_DATA(reinterpret_cast<void*>(repr_))->contents)[0]);
    }

    uint32_t closure::get_captured_size() const noexcept {
        return UNBOX(Llength(reinterpret_cast<void*>(repr_))) - 1;
    }

    value closure::get_capture(uint32_t pos) const {
        return value{from_repr_t, reinterpret_cast<auint*>(TO_DATA(reinterpret_cast<void*>(repr_))->contents)[pos + 1]};
    }

    value closure::get_capture_reference(uint32_t pos) const {
        return value{&reinterpret_cast<auint*>(TO_DATA(reinterpret_cast<void*>(repr_))->contents)[pos + 1]};
    }

    void closure::set_capture(uint32_t pos, value capture) {
        reinterpret_cast<auint*>(TO_DATA(reinterpret_cast<void*>(repr_))->contents)[pos + 1] = capture.get_repr();
    }

    auint closure::to_closure(std::span<auint> captured) {
        return reinterpret_cast<auint>(Bclosure(static_cast<aint*>(static_cast<void*>(captured.data())), BOX(captured.size())));
    }

    value::value() noexcept
        : value(from_repr_t, 0) {
    }

    value::value(from_repr, auint repr) noexcept
        : repr_(repr) {
    }

    value::value(int32_t val) noexcept
        : repr_(BOX(val)) {
    }

    value::value(const auint* val) noexcept
        : repr_(reinterpret_cast<auint>(val)) {
    }

    value::value(aggregate val) noexcept
        : repr_(val.get_repr()) {
    }

    value::value(std::string_view val)
        : repr_(to_string(val)) {
    }

    value::value(array val) noexcept
        : repr_(val.get_repr()) {
    }

    value::value(s_expr val) noexcept
        : repr_(val.get_repr()) {
    }

    value::value(closure val) noexcept
        : repr_(val.get_repr()) {
    }

    auint value::get_repr() const noexcept {
        return repr_;
    }

    bool value::is_empty() const noexcept {
        return repr_ == 0;
    }

    bool value::is_integer() const noexcept {
        return (repr_ & 1) != 0;
    }

    bool value::is_reference() const noexcept {
        return (repr_ & 1) == 0;
    }

    bool value::is_aggregate() const noexcept {
        return is_string() || is_array() || is_s_expr();
    }

    bool value::is_string() const noexcept {
        return is_reference() && get_type_header_ptr(get_obj_header_ptr(reinterpret_cast<void*>(repr_))) == STRING;
    }

    bool value::is_array() const noexcept {
        return is_reference() && get_type_header_ptr(get_obj_header_ptr(reinterpret_cast<void*>(repr_))) == ARRAY;
    }

    bool value::is_s_expr() const noexcept {
        return is_reference() && get_type_header_ptr(get_obj_header_ptr(reinterpret_cast<void*>(repr_))) == SEXP;
    }

    bool value::is_closure() const noexcept {
        return is_reference() && get_type_header_ptr(get_obj_header_ptr(reinterpret_cast<void*>(repr_))) == CLOSURE;
    }

    int32_t value::as_integer() const noexcept {
        return UNBOX(repr_);
    }

    auint* value::as_reference() const noexcept {
        return reinterpret_cast<auint*>(repr_);
    }

    aggregate value::as_aggregate() const noexcept {
        return aggregate{from_repr_t, repr_};
    }

    std::string_view value::as_string() const noexcept {
        return std::string_view{TO_DATA(reinterpret_cast<void*>(repr_))->contents};
    }

    array value::as_array() const noexcept {
        return array{from_repr_t, repr_};
    }

    s_expr value::as_s_expr() const noexcept {
        return s_expr{from_repr_t, repr_};
    }

    closure value::as_closure() const noexcept {
        return closure{from_repr_t, repr_};
    }

    value value::get_string() const noexcept {
        std::array<aint, 1> values{static_cast<aint>(repr_)};
        return value{static_cast<auint*>(Lstring(values.data()))};
    }

    auint value::to_string(std::string_view val) {
        std::array<aint, 1> values{{reinterpret_cast<aint>(const_cast<char*>(val.data()))}};
        return reinterpret_cast<auint>(Bstring(values.data()));
    }

    frame::frame() noexcept
        : base_(static_cast<uint32_t>(-1))
        , locals_size_(static_cast<uint32_t>(-1))
        , args_size_(static_cast<uint32_t>(-1))
        , is_frame_closure_(false)
        , return_address_(static_cast<uint32_t>(-1)) {
    }

    frame::frame(std::span<auint> stack, uint32_t base, uint32_t locals_size, uint32_t args_size, bool is_frame_closure) noexcept
        : stack_(stack)
        , base_(base)
        , locals_size_(locals_size)
        , args_size_(args_size)
        , is_frame_closure_(is_frame_closure)
        , return_address_(static_cast<uint32_t>(-1)) {
    }

    uint32_t frame::get_base() const noexcept {
        return base_;
    }

    uint32_t frame::get_locals_size() const noexcept {
        return locals_size_;
    }

    value frame::get_local(uint32_t pos) const {
        return value{from_repr_t, stack_[base_ + pos]};
    }

    value frame::get_local_reference(uint32_t pos) const {
        return value{&stack_[base_ + pos]};
    }

    void frame::set_local(uint32_t pos, value local) {
        stack_[base_ + pos] = local.get_repr();
    }

    uint32_t frame::get_args_size() const noexcept {
        return args_size_;
    }

    value frame::get_arg(uint32_t pos) const {
        return value{from_repr_t, stack_[base_ - args_size_ + pos]};
    }

    value frame::get_arg_reference(uint32_t pos) const {
        return value{&stack_[base_ - args_size_ + pos]};
    }

    void frame::set_arg(uint32_t pos, value arg) {
        stack_[base_ - args_size_ + pos] = arg.get_repr();
    }

    bool frame::is_closure() const noexcept {
        return is_frame_closure_;
    }

    void frame::is_closure(bool is_frame_closure) noexcept {
        is_frame_closure_ = is_frame_closure;
    }

    uint32_t frame::get_captured_vars_size() const noexcept {
        closure frame_closure(from_repr_t, stack_[base_ - args_size_ - 1]);
        return frame_closure.get_captured_size();
    }

    value frame::get_captured_var(uint32_t pos) const {
        closure frame_closure(from_repr_t, stack_[base_ - args_size_ - 1]);
        return frame_closure.get_capture(pos);
    }

    value frame::get_captured_var_reference(uint32_t pos) const {
        closure frame_closure(from_repr_t, stack_[base_ - args_size_ - 1]);
        return frame_closure.get_capture_reference(pos);
    }

    void frame::set_captured_var(uint32_t pos, value captured_var) {
        closure frame_closure(from_repr_t, stack_[base_ - args_size_ - 1]);
        frame_closure.set_capture(pos, captured_var);
    }

    uint32_t frame::get_return_address() const noexcept {
        return return_address_;
    }

    void frame::set_return_address(uint32_t return_address) noexcept {
        return_address_ = return_address;
    }

    state::state(const bytefile& file) noexcept
        : ip_(0)
        , frames_(frames_buf.begin(), 0)
        , stack_(stack_buf.begin(), file.get_global_area_size() + 2)
        , is_tmp_closure_(false)
        , bytefile_(file) {
        validate(stack_.size() < MAX_STACK_SIZE, "Stack overflow. Bytecode offset: %#X\n");
        __init();
        __gc_stack_top = reinterpret_cast<size_t>(stack_.data());
        __gc_stack_bottom = reinterpret_cast<size_t>(stack_.data() + stack_.size());
    }

    state::~state() {
        __shutdown();
    }

    bytecode state::pop_next_op() {
        ++ip_;
        validate(ip_ < bytefile_.get_code_size(), "Unexpected end of code. Bytecode offset: %#X\n");
        return bytefile_.get_code(ip_ - sizeof(bytecode));
    }

    void state::execute_binop_high() {
        int32_t res = 0;
        value rhs = pop();
        value lhs = pop();
        validate(lhs.is_integer(), "operand must be integer. Bytecode offset: %#X\n");
        int32_t lhs_int = lhs.as_integer();
        validate(rhs.is_integer(), "operand must be integer. Bytecode offset: %#X\n");
        int32_t rhs_int = rhs.as_integer();
        switch (peek_current_op()) {
            case bytecode::LOW_ADD:
                res = static_cast<int32_t>(static_cast<int64_t>(lhs_int) + static_cast<int64_t>(rhs_int));
                break;
            case bytecode::LOW_SUB:
                res = static_cast<int32_t>(static_cast<int64_t>(lhs_int) - static_cast<int64_t>(rhs_int));
                break;
            case bytecode::LOW_MUL:
                res = static_cast<int32_t>(static_cast<int64_t>(lhs_int) * static_cast<int64_t>(rhs_int));
                break;
            case bytecode::LOW_DIV:
                validate(rhs_int != 0, "division by zero. Bytecode offset: %#X\n");
                res = static_cast<int32_t>(static_cast<int64_t>(lhs_int) / static_cast<int64_t>(rhs_int));
                break;
            case bytecode::LOW_MOD:
                validate(rhs_int != 0, "division by zero. Bytecode offset: %#X\n");
                res = static_cast<int32_t>(static_cast<int64_t>(lhs_int) % static_cast<int64_t>(rhs_int));
                break;
            case bytecode::LOW_LT:
                res = (lhs_int < rhs_int);
                break;
            case bytecode::LOW_LE:
                res = (lhs_int <= rhs_int);
                break;
            case bytecode::LOW_GT:
                res = (lhs_int > rhs_int);
                break;
            case bytecode::LOW_GE:
                res = (lhs_int >= rhs_int);
                break;
            case bytecode::LOW_NE:
                res = (lhs_int != rhs_int);
                break;
            case bytecode::LOW_AND:
                res = (lhs_int && rhs_int);
                break;
            case bytecode::LOW_OR:
                res = (lhs_int || rhs_int);
                break;
            default:
                validate(false, "Unknown bytecode. Bytecode offset: %#X\n");
        }
        push(res);
    }

    void state::execute_low_eq() {
        int32_t res = 0;
        value rhs = pop();
        value lhs = pop();
        validate(lhs.is_integer() || rhs.is_integer(), "one of the operands must be integer. Bytecode offset: %#X\n");
        if (lhs.is_integer() && rhs.is_integer()) {
            int32_t lhs_int = lhs.as_integer();
            int32_t rhs_int = rhs.as_integer();
            res = (lhs_int == rhs_int);
        }
        push(res);
    }

    void state::execute_const() {
        int32_t constant = pop_next_int32();
        push(constant);
    }

    void state::execute_string() {
        int32_t string_pos = pop_next_int32();
        validate(string_pos >= 0 && string_pos < bytefile_.get_string_tab_size(), "string index out of bounds. Bytecode offset: %#X\n");
        std::string_view string = bytefile_.get_string(string_pos);
        push(string);
    }

    void state::execute_sexp() {
        int32_t tag_pos = pop_next_int32();
        int32_t elements_size = pop_next_int32();
        validate(elements_size >= 0, "elements size must be non-negative. Bytecode offset: %#X\n");
        validate(tag_pos >= 0 && tag_pos < bytefile_.get_string_tab_size(), "string index out of bounds. Bytecode offset: %#X\n");
        std::string_view tag = bytefile_.get_string(tag_pos);
        std::span<auint> elements(stack_buf.begin() + stack_.size() - elements_size, elements_size + 1);
        s_expr s_expression(tag, elements);
        pop(elements_size);
        push(s_expression);
    }

    void state::execute_sti() {
        value addr = pop();
        validate(addr.is_reference(), "STI: argument must be reference. Bytecode offset: %#X\n");
        auint* addr_ref = addr.as_reference();
        value val = pop();
        *addr_ref = val.get_repr();
        push(val);
    }

    void state::execute_sta() {
        value val = pop();
        value selector = pop();
        if (!selector.is_integer()) {
            validate(selector.is_reference(), "STA: argument must be reference. Bytecode offset: %#X\n");
            auint* addr_ref = selector.as_reference();
            *addr_ref = val.get_repr();
            push(val);
            return;
        }
        int32_t idx_int = selector.as_integer();
        value agg = pop();
        validate(agg.is_aggregate(), "STA: argument must be aggregate. Bytecode offset: %#X\n");
        aggregate agg_val = agg.as_aggregate();
        validate(idx_int >= 0 && idx_int < agg_val.get_elements_size(), "STA: index out of bounds. Bytecode offset: %#X\n");
        agg_val.set_element(idx_int, val);
        push(val);
    }

    void     state::execute_jmp() {
        uint32_t addr = pop_next_int32();
        validate(addr < bytefile_.get_code_size(), "incorrect jump destination. Bytecode offset: %#X\n");
        ip_ = addr;
    }

    bool state::execute_ret() {
        value ret = pop();
        frame current_frame = pop_frame();
        stack_ = stack_.subspan(0, current_frame.get_base() - current_frame.get_args_size());
        __gc_stack_bottom = __gc_stack_top + (current_frame.get_base() - current_frame.get_args_size()) * sizeof(auint);
        if (current_frame.is_closure()) {
            stack_ = stack_.subspan(0, stack_.size() - 1);
            __gc_stack_bottom -= sizeof(auint);
        }
        if (!has_frame()) {
            return true;
        }
        ip_ = peek_frame().get_return_address();
        push(ret);
        return false;
    }

    void state::execute_drop() {
        pop();
    }

    void state::execute_dup() {
        value val = peek();
        push(val);
    }

    void state::execute_swap() {
        value a = pop();
        value b = pop();
        push(b);
        push(a);
    }

    void state::execute_elem() {
        value idx = pop();
        validate(idx.is_integer(), "index must be integer. Bytecode offset: %#X\n");
        int32_t idx_int = idx.as_integer();
        value agg = pop();
        validate(agg.is_aggregate(), "argument must be aggregate. Bytecode offset: %#X\n");
        aggregate agg_val = agg.as_aggregate();
        validate(idx_int >= 0 && idx_int < agg_val.get_elements_size(), "index out of bounds. Bytecode offset: %#X\n");
        value val = agg_val.get_element(idx_int);
        push(val);
    }

    void state::execute_ld_global() {
        int32_t addr = pop_next_int32();
        validate(addr >= 0 && addr < get_globals_size(), "LD: global index out of bounds. Bytecode offset: %#X\n");
        value target = get_global(addr);
        push(target);
    }

    void state::execute_ld_local() {
        int32_t addr = pop_next_int32();
        frame& current_frame = peek_frame();
        validate(addr >= 0 && addr < current_frame.get_locals_size(), "LD: local index out of bounds. Bytecode offset: %#X\n");
        value target = current_frame.get_local(addr);
        push(target);
    }

    void state::execute_ld_argument() {
        int32_t addr = pop_next_int32();
        frame& current_frame = peek_frame();
        validate(addr >= 0 && addr < current_frame.get_args_size(), "LD: argument index out of bounds. Bytecode offset: %#X\n");
        value target = current_frame.get_arg(addr);
        push(target);
    }

    void state::execute_ld_captured() {
        int32_t addr = pop_next_int32();
        frame& current_frame = peek_frame();
        validate(addr >= 0 && addr < current_frame.get_captured_vars_size(), "LD: captured index out of bounds. Bytecode offset: %#X\n");
        value target = current_frame.get_captured_var(addr);
        push(target);
    }

    void state::execute_lda_global() {
        int32_t addr = pop_next_int32();
        validate(addr >= 0 && addr < get_globals_size(), "LDA: global index out of bounds. Bytecode offset: %#X\n");
        value target(get_global_reference(addr));
        push(target);
    }

    void state::execute_lda_local() {
        int32_t addr = pop_next_int32();
        frame& current_frame = peek_frame();
        validate(addr >= 0 && addr < current_frame.get_locals_size(), "LDA: local index out of bounds. Bytecode offset: %#X\n");
        value target(current_frame.get_local_reference(addr));
        push(target);
    }

    void state::execute_lda_argument() {
        int32_t addr = pop_next_int32();
        frame& current_frame = peek_frame();
        validate(addr >= 0 && addr < current_frame.get_args_size(), "LDA: argument index out of bounds. Bytecode offset: %#X\n");
        value target(current_frame.get_arg_reference(addr));
        push(target);
    }

    void state::execute_lda_captured() {
        int32_t addr = pop_next_int32();
        frame& current_frame = peek_frame();
        validate(addr >= 0 && addr < current_frame.get_captured_vars_size(), "LDA: captured index out of bounds. Bytecode offset: %#X\n");
        value target(current_frame.get_captured_var_reference(addr));
        push(target);
    }

    void state::execute_st_global() {
        value val = pop();
        int32_t addr = pop_next_int32();
        validate(addr >= 0 && addr < get_globals_size(), "ST: global index out of bounds. Bytecode offset: %#X\n");
        set_global(addr, val);
        push(val);
    }

    void state::execute_st_local() {
        value val = pop();
        int32_t addr = pop_next_int32();
        frame& current_frame = peek_frame();
        validate(addr >= 0 && addr < current_frame.get_locals_size(), "ST: local index out of bounds. Bytecode offset: %#X\n");
        current_frame.set_local(addr, val);
        push(val);
    }

    void state::execute_st_argument() {
        value val = pop();
        int32_t addr = pop_next_int32();
        frame& current_frame = peek_frame();
        validate(addr >= 0 && addr < current_frame.get_args_size(), "ST: argument index out of bounds. Bytecode offset: %#X\n");
        current_frame.set_arg(addr, val);
        push(val);
    }

    void state::execute_st_captured() {
        value val = pop();
        int32_t addr = pop_next_int32();
        frame& current_frame = peek_frame();
        validate(addr >= 0 && addr < current_frame.get_captured_vars_size(), "ST: captured index out of bounds. Bytecode offset: %#X\n");
        current_frame.set_captured_var(addr, val);
        push(val);
    }

    void state::execute_cjmp(bool nz) {
        uint32_t addr = pop_next_int32();
        validate(addr < bytefile_.get_code_size(), "incorrect cjmpz destination. Bytecode offset: %#X\n");
        value cond = pop();
        validate(cond.is_integer(), "argument must be integer. Bytecode offset: %#X\n");
        int32_t cond_int = cond.as_integer();
        if (cond_int == nz) {
            ip_ = addr;
        }
    }

    void state::execute_begin() {
        int32_t args_size = pop_next_int32();
        int32_t locals_size = pop_next_int32();
        validate(args_size >= 0, "args size must be non-negative. Bytecode offset: %#X\n");
        validate(locals_size >= 0, "locals size must be non-negative. Bytecode offset: %#X\n");
        frame new_frame(stack_, stack_.size(), locals_size, args_size, is_tmp_closure_);
        is_tmp_closure_ = false;
        validate(stack_.size() + locals_size <= MAX_STACK_SIZE, "Stack overflow. Bytecode offset: %#X\n");
        stack_ = std::span(stack_buf.begin(), stack_.size() + locals_size);
        __gc_stack_bottom += locals_size * sizeof(auint);
        push_frame(new_frame);
    }

    void state::execute_cbegin() {
        int32_t args_size = pop_next_int32();
        int32_t locals_size = pop_next_int32();
        validate(args_size >= 0, "args size must be non-negative. Bytecode offset: %#X\n");
        validate(locals_size >= 0, "locals size must be non-negative. Bytecode offset: %#X\n");
        frame new_frame(stack_, stack_.size(), locals_size, args_size, true);
        validate(stack_.size() + locals_size <= MAX_STACK_SIZE, "Stack overflow. Bytecode offset: %#X\n");
        stack_ = std::span(stack_buf.begin(), stack_.size() + locals_size);
        __gc_stack_bottom += locals_size * sizeof(auint);
        push_frame(new_frame);
    }

    void state::execute_closure() {
        int32_t addr = pop_next_int32();
        int32_t captured_size = pop_next_int32();
        validate(addr >= 0 && addr < bytefile_.get_code_size(), "incorrect closure address. Bytecode offset: %#X\n");
        validate(captured_size >= 0, "captured size must be non-negative. Bytecode offset: %#X\n");
        push(addr);
        const frame& current_frame = peek_frame();
        for (int32_t i = 0; i < captured_size; ++i) {
            int8_t capture_type = pop_next_int8();
            int32_t capture_addr = pop_next_int32();
            validate(capture_type >= 0x00 && capture_type <= 0x03, "invalid varspec. Bytecode offset: %#X\n");
            value capture;
            switch (capture_type) {
                case 0x00:
                    validate(capture_addr >= 0 && capture_addr < get_globals_size(), "CLOSURE: global index out of bounds. Bytecode offset: %#X\n");
                    capture = get_global(capture_addr);
                    break;
                case 0x01:
                    validate(capture_addr >= 0 && capture_addr < current_frame.get_locals_size(), "CLOSURE: local index out of bounds. Bytecode offset: %#X\n");
                    capture = current_frame.get_local(capture_addr);
                    break;
                case 0x02:
                    validate(capture_addr >= 0 && capture_addr < current_frame.get_args_size(), "CLOSURE: argument index out of bounds. Bytecode offset: %#X\n");
                    capture = current_frame.get_arg(capture_addr);
                    break;
                case 0x03:
                    validate(capture_addr >= 0 && capture_addr < current_frame.get_captured_vars_size(), "CLOSURE: captured index out of bounds. Bytecode offset: %#X\n");
                    capture = current_frame.get_captured_var(capture_addr);
                    break;
                default:
                    validate(false, "invalid varspec for closure. Bytecode offset: %#X\n");
            }
            push(capture);
        }
        std::span<auint> captured = stack_.subspan(stack_.size() - captured_size - 1, captured_size + 1);
        closure tmp_closure(captured);
        pop(captured_size + 1);
        push(tmp_closure);
    }

    void state::execute_callc() {
        int32_t args_size = pop_next_int32();
        validate(args_size >= 0, "args size must be non-negative. Bytecode offset: %#X\n");
        frame& current_frame = peek_frame();
        current_frame.set_return_address(ip_);
        value target = peek(args_size);
        validate(target.is_closure(), "argument must be closure. Bytecode offset: %#X\n");
        closure target_closure = target.as_closure();
        validate(target_closure.get_code_offset() < bytefile_.get_code_size(), "incorrect callc destination. Bytecode offset: %#X\n");
        ip_ = target_closure.get_code_offset();
        is_tmp_closure_ = true;
        validate(peek_next_op() == bytecode::BEGIN || peek_next_op() == bytecode::CBEGIN, "destination instruction must be begin or cbegin. Bytecode offset: %#X\n");
    }

    void state::execute_call() {
        int32_t addr = pop_next_int32();
        int32_t args_size = pop_next_int32();
        validate(addr >= 0 && addr < bytefile_.get_code_size(), "incorrect closure address. Bytecode offset: %#X\n");
        validate(args_size >= 0, "args size must be non-negative. Bytecode offset: %#X\n");
        frame& current_frame = peek_frame();
        current_frame.set_return_address(ip_);
        ip_ = addr;
        is_tmp_closure_ = false;
        validate(peek_next_op() == bytecode::BEGIN, "destination instruction must be begin. Bytecode offset: %#X\n");
    }

    void state::execute_tag() {
        int32_t tag_pos = pop_next_int32();
        int32_t elements_size = pop_next_int32();
        validate(tag_pos >= 0 && tag_pos < bytefile_.get_string_tab_size(), "string index out of bounds. Bytecode offset: %#X\n");
        validate(elements_size >= 0, "elements size must be non-negative. Bytecode offset: %#X\n");
        value res(0);
        value val = pop();
        if (val.is_s_expr()) {
            std::string_view tag = bytefile_.get_string(tag_pos);
            res = value{from_repr_t, static_cast<auint>(Btag(static_cast<void*>(val.as_reference()), LtagHash(const_cast<char*>(tag.data())), BOX(elements_size)))};
        }
        push(res);
    }

    void state::execute_array() {
        int32_t elements_size = pop_next_int32();
        validate(elements_size >= 0, "elements size must be non-negative. Bytecode offset: %#X\n");
        value val = pop();
        value res(from_repr_t, static_cast<auint>(Barray_patt(static_cast<void*>(val.as_reference()), BOX(elements_size))));
        push(res);
    }

    void state::execute_fail() {
        uint32_t line_number = pop_next_int32();
        uint32_t column_number = pop_next_int32();
        value val = pop();
        Bmatch_failure(static_cast<void*>(val.as_reference()), const_cast<char*>(bytefile_.get_name().data()), static_cast<aint>(line_number), static_cast<aint>(column_number));
    }

    void state::execute_line() {
        static_cast<void>(pop_next_int32());
    }

    void state::execute_patt_str() {
        value rhs = pop();
        value lhs = pop();
        value res(from_repr_t, static_cast<auint>(Bstring_patt(static_cast<void*>(lhs.as_reference()), static_cast<void*>(rhs.as_reference()))));
        push(res);
    }

    void state::execute_patt_string() {
        value val = pop();
        value res(from_repr_t, static_cast<auint>(Bstring_tag_patt(static_cast<void*>(val.as_reference()))));
        push(res);
    }

    void state::execute_patt_array() {
        value val = pop();
        value res(from_repr_t, static_cast<auint>(Barray_tag_patt(static_cast<void*>(val.as_reference()))));
        push(res);
    }

    void state::execute_patt_sexp() {
        value val = pop();
        value res(from_repr_t, static_cast<auint>(Bsexp_tag_patt(static_cast<void*>(val.as_reference()))));
        push(res);
    }

    void state::execute_patt_ref() {
        value val = pop();
        value res(from_repr_t, static_cast<auint>(Bboxed_patt(static_cast<void*>(val.as_reference()))));
        push(res);
    }

    void state::execute_patt_val() {
        value val = pop();
        value res(from_repr_t, static_cast<auint>(Bunboxed_patt(static_cast<void*>(val.as_reference()))));
        push(res);
    }

    void state::execute_patt_fun() {
        value val = pop();
        value res(from_repr_t, static_cast<auint>(Bclosure_tag_patt(static_cast<void*>(val.as_reference()))));
        push(res);
    }

    void state::execute_call_lread() {
        value val(from_repr_t, static_cast<auint>(Lread()));
        push(val);
    }

    void state::execute_call_lwrite() {
        value val = pop();
        validate(val.is_integer(), "invalid output. Bytecode offset: %#X\n");
        Lwrite(static_cast<aint>(val.get_repr()));
        value ret;
        push(ret);
    }

    void state::execute_call_llength() {
        value agg = pop();
        validate(agg.is_aggregate(), "argument must be aggregate. Bytecode offset: %#X\n");
        int32_t length = static_cast<int32_t>(agg.as_aggregate().get_elements_size());
        push(length);
    }

    void state::execute_call_lstring() {
        value val = pop().get_string();
        push(val);
    }

    void state::execute_call_barray() {
        int32_t elements_size = pop_next_int32();
        validate(elements_size >= 0, "elements size must be non-negative. Bytecode offset: %#X\n");
        std::span<auint> elements = stack_.subspan(stack_.size() - elements_size, elements_size);
        array arr(elements);
        pop(elements_size);
        push(arr);
    }

    void state::validate(bool condition, std::string_view message) const noexcept {
        if (!condition) {
            failure(const_cast<char*>(message.data()), ip_);
        }
    }

    bytecode state::peek_current_op() const {
        return bytefile_.get_code(ip_ - sizeof(bytecode));
    }

    bytecode state::peek_next_op() const {
        return bytefile_.get_code(ip_);
    }

    int8_t state::pop_next_int8() {
        ++ip_;
        validate(ip_ < bytefile_.get_code_size(), "Unexpected end of code. Bytecode offset: %#X\n");
        return bytefile_.get_int8(ip_ - sizeof(uint8_t));
    }

    int32_t state::pop_next_int32() {
        ip_ += sizeof(int32_t);
        validate(ip_ < bytefile_.get_code_size(), "Unexpected end of code. Bytecode offset: %#X\n");
        return bytefile_.get_int32(ip_ - sizeof(int32_t));
    }

    bool state::has_frame() const noexcept {
        return !frames_.empty();
    }

    const frame& state::peek_frame() const {
        return frames_.back();
    }

    frame& state::peek_frame() {
        return frames_.back();
    }

    void state::push_frame(const frame& frame) {
        validate(frames_.size() + 1 <= MAX_FRAMES_SIZE, "Frame stack overflow. Bytecode offset: %#X\n");
        frames_ = std::span{frames_buf.begin(), frames_.size() + 1};
        frames_[frames_.size() - 1] = frame;
    }

    frame state::pop_frame() {
        validate(!frames_.empty(), "Frame stack underflow. Bytecode offset: %#X\n");
        frame current_frame = frames_.back();
        frames_ = frames_.subspan(0, frames_.size() - 1);
        return current_frame;
    }

    value state::peek(uint32_t offset) const {
        return value{from_repr_t, stack_[stack_.size() - offset - 1]};
    }

    void state::push(value val) {
        validate(stack_.size() + 1 <= MAX_STACK_SIZE, "Stack overflow. Bytecode offset: %#X\n");
        stack_ = std::span(stack_buf.begin(), stack_.size() + 1);
        stack_[stack_.size() - 1] = val.get_repr();
        __gc_stack_bottom += sizeof(auint);
    }

    value state::pop() {
        validate(!stack_.empty(), "Stack underflow. Bytecode offset: %#X\n");
        value val(from_repr_t, stack_.back());
        stack_ = stack_.subspan(0, stack_.size() - 1);
        __gc_stack_bottom -= sizeof(auint);
        return val;
    }

    void state::pop(uint32_t count) {
        validate(stack_.size() >= count, "Stack underflow. Bytecode offset: %#X\n");
        stack_ = stack_.subspan(0, stack_.size() - count);
        __gc_stack_bottom -= count * sizeof(auint);
    }

    uint32_t state::get_globals_size() const noexcept {
        return bytefile_.get_global_area_size();
    }

    value state::get_global(uint32_t pos) const {
        return value{from_repr_t, stack_[pos]};
    }

    value state::get_global_reference(uint32_t pos) const {
        return value{&stack_[pos]};
    }

    void state::set_global(uint32_t pos, value global) {
        stack_[pos] = global.get_repr();
    }

    void interpret(const bytefile& file) {
        state interpreter_state(file);
        while (true) {
            switch (interpreter_state.pop_next_op()) {
                case bytecode::LOW_ADD:
                case bytecode::LOW_SUB:
                case bytecode::LOW_MUL:
                case bytecode::LOW_DIV:
                case bytecode::LOW_MOD:
                case bytecode::LOW_LT:
                case bytecode::LOW_LE:
                case bytecode::LOW_GT:
                case bytecode::LOW_GE:
                case bytecode::LOW_NE:
                case bytecode::LOW_AND:
                case bytecode::LOW_OR:
                    interpreter_state.execute_binop_high();
                    break;
                case bytecode::LOW_EQ:
                    interpreter_state.execute_low_eq();
                    break;
                case bytecode::CONST:
                    interpreter_state.execute_const();
                    break;
                case bytecode::STRING:
                    interpreter_state.execute_string();
                    break;
                case bytecode::SEXP:
                    interpreter_state.execute_sexp();
                    break;
                case bytecode::STI:
                    interpreter_state.execute_sti();
                    break;
                case bytecode::STA:
                    interpreter_state.execute_sta();
                    break;
                case bytecode::JMP:
                    interpreter_state.execute_jmp();
                    break;
                case bytecode::END:
                case bytecode::RET:
                    if (interpreter_state.execute_ret()) {
                        return;
                    }
                    break;
                case bytecode::DROP:
                    interpreter_state.execute_drop();
                    break;
                case bytecode::DUP:
                    interpreter_state.execute_dup();
                    break;
                case bytecode::SWAP:
                    interpreter_state.execute_swap();
                    break;
                case bytecode::ELEM:
                    interpreter_state.execute_elem();
                    break;
                case bytecode::LD_GLOBAL:
                    interpreter_state.execute_ld_global();
                    break;
                case bytecode::LD_LOCAL:
                    interpreter_state.execute_ld_local();
                    break;
                case bytecode::LD_ARGUMENT:
                    interpreter_state.execute_ld_argument();
                    break;
                case bytecode::LD_CAPTURED:
                    interpreter_state.execute_ld_captured();
                    break;
                case bytecode::LDA_GLOBAL:
                    interpreter_state.execute_lda_global();
                    break;
                case bytecode::LDA_LOCAL:
                    interpreter_state.execute_lda_local();
                    break;
                case bytecode::LDA_ARGUMENT:
                    interpreter_state.execute_lda_argument();
                    break;
                case bytecode::LDA_CAPTURED:
                    interpreter_state.execute_lda_captured();
                    break;
                case bytecode::ST_GLOBAL:
                    interpreter_state.execute_st_global();
                    break;
                case bytecode::ST_LOCAL:
                    interpreter_state.execute_st_local();
                    break;
                case bytecode::ST_ARGUMENT:
                    interpreter_state.execute_st_argument();
                    break;
                case bytecode::ST_CAPTURED:
                    interpreter_state.execute_st_captured();
                    break;
                case bytecode::CJMPZ:
                    interpreter_state.execute_cjmp(false);
                    break;
                case bytecode::CJMPNZ:
                    interpreter_state.execute_cjmp(true);
                    break;
                case bytecode::BEGIN:
                    interpreter_state.execute_begin();
                    break;
                case bytecode::CBEGIN:
                    interpreter_state.execute_cbegin();
                    break;
                case bytecode::CLOSURE:
                    interpreter_state.execute_closure();
                    break;
                case bytecode::CALLC:
                    interpreter_state.execute_callc();
                    break;
                case bytecode::CALL:
                    interpreter_state.execute_call();
                    break;
                case bytecode::TAG:
                    interpreter_state.execute_tag();
                    break;
                case bytecode::ARRAY:
                    interpreter_state.execute_array();
                    break;
                case bytecode::FAIL:
                    interpreter_state.execute_fail();
                    return;
                case bytecode::LINE:
                    interpreter_state.execute_line();
                    break;
                case bytecode::PATT_STR:
                    interpreter_state.execute_patt_str();
                    break;
                case bytecode::PATT_STRING:
                    interpreter_state.execute_patt_string();
                    break;
                case bytecode::PATT_ARRAY:
                    interpreter_state.execute_patt_array();
                    break;
                case bytecode::PATT_SEXP:
                    interpreter_state.execute_patt_sexp();
                    break;
                case bytecode::PATT_REF:
                    interpreter_state.execute_patt_ref();
                    break;
                case bytecode::PATT_VAL:
                    interpreter_state.execute_patt_val();
                    break;
                case bytecode::PATT_FUN:
                    interpreter_state.execute_patt_fun();
                    break;
                case bytecode::CALL_LREAD:
                    interpreter_state.execute_call_lread();
                    break;
                case bytecode::CALL_LWRITE:
                    interpreter_state.execute_call_lwrite();
                    break;
                case bytecode::CALL_LLENGTH:
                    interpreter_state.execute_call_llength();
                    break;
                case bytecode::CALL_LSTRING:
                    interpreter_state.execute_call_lstring();
                    break;
                case bytecode::CALL_BARRAY:
                    interpreter_state.execute_call_barray();
                    break;
                case bytecode::STOP:
                    return;
                default:
                    interpreter_state.validate(false, "unknown bytecode. Bytecode offset: %#X\n");
            }
        }
    }

}
