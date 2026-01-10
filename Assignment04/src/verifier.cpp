#include "verifier.h"

#include <optional>

#include "stack.h"

namespace assignment_04 {

    stack_size::stack_size() noexcept
        : stack_size(NO_VALUE) {
    }

    stack_size::stack_size(int32_t repr) noexcept
        : repr_(repr) {
    }

    stack_size::stack_size(from_initial, uint32_t val) noexcept
        : stack_size(-1 * static_cast<int32_t>(val) - 2) {
    }

    stack_size::stack_size(from_processed, uint32_t val) noexcept
        : stack_size(static_cast<int32_t>(val)) {
    }

    workset_entry::workset_entry(auint repr) noexcept
        : repr_(repr) {
    }

    workset_entry::workset_entry(from_instruction, uint32_t val) noexcept
        : workset_entry(static_cast<auint>(val) << 1) {
    }

    workset_entry::workset_entry(from_frame, uint32_t val) noexcept
        : workset_entry((static_cast<auint>(val) << 1) | 1) {
    }

    verifier::verifier(bytefile& file, public_symbol entrypoint)
        : addr_(0)
        , frames_depth_(0)
        , current_frame_addr_(entrypoint.get_address())
        , current_frame_stack_size_(0)
        , stack_sizes_(file.get_code_size())
        , current_stack_size_(0)
        , workset_(stack_buf.begin(), 0)
        , bytefile_(file)
        , entrypoint_(entrypoint) {
    }

    void verifier::traverse_bytecode() {
        push(workset_entry{from_instruction_t, entrypoint_.get_address()});
        while (!workset_.empty()) {
            if (peek().is_frame()) {
                current_frame_addr_ = pop().get_value();
                current_frame_stack_size_ = pop().get_value();
                --frames_depth_;
                continue;
            }
            addr_ = pop().get_value();
            if (stack_sizes_.at(addr_).is_initial()) {
                current_stack_size_ = static_cast<int32_t>(stack_sizes_.at(addr_).get_value());
            }
            current_stack_size_ += get_stack_delta();
            validate(current_stack_size_ >= 0, "Stack underflow. Bytecode offset: %#X\n");
            validate(current_stack_size_ < MAX_STACK_SIZE, "Stack overflow. Bytecode offset: %#X\n");
            if (stack_sizes_.at(addr_).is_processed()) {
                if (!workset_.empty() && peek().is_instruction()) {
                    uint32_t next_addr = peek().get_value();
                    current_stack_size_ = static_cast<int32_t>(stack_sizes_.at(next_addr).get_value());
                    validate(stack_sizes_.at(next_addr).has_value(), "Ill-formed control-flow. Bytecode offset: %#X\n");
                    if (stack_sizes_.at(next_addr).is_processed()) {
                        stack_sizes_[next_addr] = stack_size{from_initial_t, static_cast<uint32_t>(current_stack_size_)};
                    }
                }
                continue;
            }
            stack_sizes_[addr_] = stack_size{from_processed_t, static_cast<uint32_t>(current_stack_size_)};
            current_frame_stack_size_ = current_stack_size_ > static_cast<int32_t>(current_frame_stack_size_) ? static_cast<uint32_t>(current_stack_size_) : current_frame_stack_size_;
            bytecode op = pop_next_op();
            if (op != bytecode::BEGIN && op != bytecode::CBEGIN) {
                validate(frames_depth_ > 0, "CALL/CALLC destination instruction must be BEGIN or CBEGIN. Bytecode offset: %#X\n");
            }
            visit_op();
            if (op != bytecode::END && op != bytecode::RET && op != bytecode::FAIL) {
                push(workset_entry{from_instruction_t, addr_});
            }
        }
    }

    bytecode verifier::peek_current_op() const {
        return bytefile_.get_code(addr_ - sizeof(bytecode));
    }

    bytecode verifier::peek_next_op() const {
        return bytefile_.get_code(addr_);
    }

    bytecode verifier::pop_next_op() {
        ++addr_;
        validate(addr_ < bytefile_.get_code_size(), "Unexpected end of code. Bytecode offset: %#X\n");
        return bytefile_.get_code(addr_ - sizeof(bytecode));
    }

    varspec verifier::pop_next_varspec() {
        ++addr_;
        validate(addr_ < bytefile_.get_code_size(), "Unexpected end of code. Bytecode offset: %#X\n");
        return bytefile_.get_varspec(addr_ - sizeof(varspec));
    }

    int32_t verifier::pop_next_int32() {
        addr_ += sizeof(int32_t);
        validate(addr_ < bytefile_.get_code_size(), "Unexpected end of code. Bytecode offset: %#X\n");
        return bytefile_.get_int32(addr_ - sizeof(int32_t));
    }

    workset_entry verifier::peek() const {
        return workset_entry{workset_[workset_.size() - 1]};
    }

    void verifier::push(workset_entry val) {
        workset_ = std::span{stack_buf.begin(), workset_.size() + 1};
        workset_[workset_.size() - 1] = val.get_repr();
    }

    workset_entry verifier::pop() {
        workset_entry val(workset_.back());
        workset_ = workset_.subspan(0, workset_.size() - 1);
        return val;
    }

        int32_t verifier::get_stack_delta() const {
        switch (peek_next_op()) {
            case bytecode::LOW_ADD:
            case bytecode::LOW_SUB:
            case bytecode::LOW_MUL:
            case bytecode::LOW_DIV:
            case bytecode::LOW_MOD:
            case bytecode::LOW_LT:
            case bytecode::LOW_LE:
            case bytecode::LOW_GT:
            case bytecode::LOW_GE:
            case bytecode::LOW_EQ:
            case bytecode::LOW_NE:
            case bytecode::LOW_AND:
            case bytecode::LOW_OR:
            case bytecode::STI:
            case bytecode::DROP:
            case bytecode::ELEM:
            case bytecode::CJMPZ:
            case bytecode::CJMPNZ:
            case bytecode::FAIL:
            case bytecode::PATT_STR:
                return -1;
            case bytecode::CONST:
            case bytecode::STRING:
            case bytecode::DUP:
            case bytecode::LD_GLOBAL:
            case bytecode::LD_LOCAL:
            case bytecode::LD_ARGUMENT:
            case bytecode::LD_CAPTURED:
            case bytecode::LDA_GLOBAL:
            case bytecode::LDA_LOCAL:
            case bytecode::LDA_ARGUMENT:
            case bytecode::LDA_CAPTURED:
            case bytecode::CALL_LREAD:
                return 1;
            case bytecode::SEXP:
            case bytecode::CALL: {
                int32_t elements_size = bytefile_.get_int32(addr_ + sizeof(bytecode) + sizeof(int32_t));
                return 1 - elements_size;
            }
            case bytecode::JMP:
            case bytecode::END:
            case bytecode::RET:
            case bytecode::SWAP:
            case bytecode::ST_GLOBAL:
            case bytecode::ST_LOCAL:
            case bytecode::ST_ARGUMENT:
            case bytecode::ST_CAPTURED:
            case bytecode::BEGIN:
            case bytecode::CBEGIN:
            case bytecode::TAG:
            case bytecode::ARRAY:
            case bytecode::LINE:
            case bytecode::PATT_STRING:
            case bytecode::PATT_ARRAY:
            case bytecode::PATT_SEXP:
            case bytecode::PATT_REF:
            case bytecode::PATT_VAL:
            case bytecode::PATT_FUN:
            case bytecode::CALL_LWRITE:
            case bytecode::CALL_LLENGTH:
            case bytecode::CALL_LSTRING:
            case bytecode::STOP:
                return 0;
            case bytecode::CLOSURE: {
                int32_t captured_size = bytefile_.get_int32(addr_ + sizeof(bytecode) + sizeof(int32_t));
                return captured_size + 1;
            }
            case bytecode::CALLC: {
                int32_t args_size = bytefile_.get_int32(addr_ + sizeof(bytecode));
                return -args_size;
            }
            case bytecode::CALL_BARRAY: {
                int32_t elements_size = bytefile_.get_int32(addr_ + sizeof(bytecode));
                return 1 - elements_size;
            }
            case bytecode::STA:
                validate(false, "STA: unable to check statically. Bytecode offset: %#X\n");
                return 0;
            default:
                validate(false, "Unknown bytecode. Bytecode offset: %#X\n");
                return 0;
        }
    }

    void verifier::visit_op() {
        switch (peek_current_op()) {
            case bytecode::CONST:
            case bytecode::LD_CAPTURED:
            case bytecode::LDA_CAPTURED:
            case bytecode::ST_CAPTURED:
            case bytecode::ARRAY:
            case bytecode::LINE:
                process_single_arg();
                break;
            case bytecode::STRING:
                process_string();
                break;
            case bytecode::SEXP:
                process_sexp();
                break;
            case bytecode::JMP:
                process_jmp();
                break;
            case bytecode::END:
            case bytecode::RET:
                process_ret();
                break;
            case bytecode::LD_GLOBAL:
            case bytecode::LDA_GLOBAL:
            case bytecode::ST_GLOBAL:
                process_global();
                break;
            case bytecode::LD_LOCAL:
            case bytecode::LDA_LOCAL:
            case bytecode::ST_LOCAL:
                process_local();
                break;
            case bytecode::LD_ARGUMENT:
            case bytecode::LDA_ARGUMENT:
            case bytecode::ST_ARGUMENT:
                process_argument();
                break;
            case bytecode::CJMPZ:
            case bytecode::CJMPNZ:
                process_cjmp();
                break;
            case bytecode::BEGIN:
            case bytecode::CBEGIN:
                process_begin();
                break;
            case bytecode::CLOSURE:
                process_closure();
                break;
            case bytecode::CALLC:
                process_callc();
                break;
            case bytecode::CALL:
                process_call();
                break;
            case bytecode::TAG:
                process_tag();
                break;
            case bytecode::FAIL:
                process_fail();
                break;
            case bytecode::CALL_BARRAY:
                process_call_barray();
                break;
            case bytecode::LOW_ADD:
            case bytecode::LOW_SUB:
            case bytecode::LOW_MUL:
            case bytecode::LOW_DIV:
            case bytecode::LOW_MOD:
            case bytecode::LOW_LT:
            case bytecode::LOW_LE:
            case bytecode::LOW_GT:
            case bytecode::LOW_GE:
            case bytecode::LOW_EQ:
            case bytecode::LOW_NE:
            case bytecode::LOW_AND:
            case bytecode::LOW_OR:
            case bytecode::STI:
            case bytecode::STA:
            case bytecode::DROP:
            case bytecode::DUP:
            case bytecode::SWAP:
            case bytecode::ELEM:
            case bytecode::PATT_STR:
            case bytecode::PATT_STRING:
            case bytecode::PATT_ARRAY:
            case bytecode::PATT_SEXP:
            case bytecode::PATT_REF:
            case bytecode::PATT_VAL:
            case bytecode::PATT_FUN:
            case bytecode::CALL_LREAD:
            case bytecode::CALL_LWRITE:
            case bytecode::CALL_LLENGTH:
            case bytecode::CALL_LSTRING:
            case bytecode::STOP:
                break;
            default:
                validate(false, "Unknown bytecode. Bytecode offset: %#X\n");
        }
    }

    void verifier::process_single_arg() {
        static_cast<void>(pop_next_int32());
    }

    void verifier::process_string() {
        int32_t string_pos = pop_next_int32();
        validate(string_pos >= 0 && string_pos < bytefile_.get_string_tab_size(), "STRING: index out of bounds. Bytecode offset: %#X\n");
    }

    void verifier::process_sexp() {
        int32_t tag_pos = pop_next_int32();
        int32_t elements_size = pop_next_int32();
        validate(elements_size >= 0, "SEXP: elements size must be non-negative. Bytecode offset: %#X\n");
        validate(tag_pos >= 0 && tag_pos < bytefile_.get_string_tab_size(), "SEXP: index out of bounds. Bytecode offset: %#X\n");
    }

    void verifier::process_jmp() {
        int32_t addr = pop_next_int32();
        validate(addr >= 0 && addr < bytefile_.get_code_size(), "JMP: incorrect destination. Bytecode offset: %#X\n");
        if (!stack_sizes_.at(addr).has_value()) {
            stack_sizes_[addr] = stack_size{from_initial_t, static_cast<uint32_t>(current_stack_size_)};
        } else {
            validate(stack_sizes_.at(addr_ - sizeof(bytecode) - sizeof(int32_t)).get_value() == static_cast<uint32_t>(current_stack_size_), "JMP: stack size mismatch at merge point. Bytecode offset: %#X\n");
        }
        addr_ = addr;
    }

    void verifier::process_ret() {
        if (!workset_.empty() && peek().is_frame()) {
            pop();
            pop();
            --frames_depth_;
        }
        uint32_t locals_size = get_locals_size();
        locals_size = (current_frame_stack_size_ << 16) | (locals_size & 0xFFFF);
        set_locals_size(locals_size);
        if (workset_.empty()) {
            return;
        }
        uint32_t next_addr = peek().get_value();
        current_stack_size_ = static_cast<int32_t>(stack_sizes_.at(next_addr).get_value());
        if (stack_sizes_.at(next_addr).is_processed()) {
            stack_sizes_[next_addr] = stack_size{from_initial_t, static_cast<uint32_t>(current_stack_size_)};
        }
    }

    void verifier::process_global() {
        int32_t addr = pop_next_int32();
        validate(addr >= 0 && addr < get_globals_size(), "LD/LDA/ST: global index out of bounds. Bytecode offset: %#X\n");
    }

    void verifier::process_local() {
        int32_t addr = pop_next_int32();
        validate(addr >= 0 && addr < get_locals_size(), "LD/LDA/ST: local index out of bounds. Bytecode offset: %#X\n");
    }

    void verifier::process_argument() {
        int32_t addr = pop_next_int32();
        validate(addr >= 0 && addr < get_args_size(), "LD/LDA/ST: argument index out of bounds. Bytecode offset: %#X\n");
    }

    void verifier::process_cjmp() {
        int32_t addr = pop_next_int32();
        validate(addr >= 0 && addr < bytefile_.get_code_size(), "CJMPZ/CJMPNZ: incorrect destination. Bytecode offset: %#X\n");
        if (!stack_sizes_.at(addr).has_value()) {
            stack_sizes_[addr] = stack_size{from_initial_t, static_cast<uint32_t>(current_stack_size_)};
            push(workset_entry{from_instruction_t, static_cast<uint32_t>(addr)});
        } else {
            validate(stack_sizes_.at(addr_ - sizeof(bytecode) - sizeof(int32_t)).get_value() == static_cast<uint32_t>(current_stack_size_), "JMP: stack size mismatch at merge point. Bytecode offset: %#X\n");
        }
    }

    void verifier::process_begin() {
        int32_t args_size = pop_next_int32();
        int32_t locals_size = pop_next_int32();
        validate(args_size >= 0, "BEGIN/CBEGIN: args size must be non-negative. Bytecode offset: %#X\n");
        validate(locals_size >= 0, "BEGIN/CBEGIN: locals size must be non-negative. Bytecode offset: %#X\n");
        push(workset_entry{from_frame_t, current_frame_stack_size_});
        push(workset_entry{from_frame_t, current_frame_addr_});
        current_frame_addr_ = addr_ - sizeof(bytecode) - sizeof(int32_t) - sizeof(int32_t);
        ++frames_depth_;
    }

    void verifier::process_closure() {
        int32_t addr = pop_next_int32();
        int32_t captured_size = pop_next_int32();
        validate(addr >= 0 && addr < bytefile_.get_code_size(), "CLOSURE: incorrect address. Bytecode offset: %#X\n");
        validate(captured_size >= 0, "CLOSURE: captured size must be non-negative. Bytecode offset: %#X\n");
        for (int32_t i = 0; i < captured_size; ++i) {
            varspec capture_type = pop_next_varspec();
            int32_t capture_addr = pop_next_int32();
            switch (capture_type) {
                case varspec::GLOBAL:
                    validate(capture_addr >= 0 && capture_addr < get_globals_size(), "CLOSURE: global index out of bounds. Bytecode offset: %#X\n");
                    break;
                case varspec::LOCAL:
                    validate(capture_addr >= 0 && capture_addr < get_locals_size(), "CLOSURE: local index out of bounds. Bytecode offset: %#X\n");
                    break;
                case varspec::ARGUMENT:
                    validate(capture_addr >= 0 && capture_addr < get_args_size(), "CLOSURE: argument index out of bounds. Bytecode offset: %#X\n");
                    break;
                case varspec::CAPTURED:
                    break;
                default:
                    validate(false, "CLOSURE: invalid varspec. Bytecode offset: %#X\n");
            }
        }
    }

    void verifier::process_callc() {
        int32_t args_size = pop_next_int32();
        validate(args_size >= 0, "CALLC: args size must be non-negative. Bytecode offset: %#X\n");
    }

    void verifier::process_call() {
        int32_t addr = pop_next_int32();
        int32_t args_size = pop_next_int32();
        validate(addr >= 0 && addr < bytefile_.get_code_size(), "CALL: incorrect destination. Bytecode offset: %#X\n");
        validate(args_size >= 0, "CALL: args size must be non-negative. Bytecode offset: %#X\n");
        stack_sizes_[addr] = stack_size{from_initial_t, static_cast<uint32_t>(current_stack_size_)};
        push(workset_entry{from_instruction_t, static_cast<uint32_t>(addr)});
    }

    void verifier::process_tag() {
        int32_t tag_pos = pop_next_int32();
        int32_t elements_size = pop_next_int32();
        validate(tag_pos >= 0 && tag_pos < bytefile_.get_string_tab_size(), "TAG: index out of bounds. Bytecode offset: %#X\n");
        validate(elements_size >= 0, "TAG: elements size must be non-negative. Bytecode offset: %#X\n");
    }

    void verifier::process_fail() {
        static_cast<void>(pop_next_int32());
        static_cast<void>(pop_next_int32());
        if (!workset_.empty() && peek().is_frame()) {
            pop();
            pop();
            --frames_depth_;
        }
        uint32_t locals_size = get_locals_size();
        locals_size = (current_frame_stack_size_ << 16) | (locals_size & 0xFFFF);
        set_locals_size(locals_size);
        if (workset_.empty()) {
            return;
        }
        uint32_t next_addr = peek().get_value();
        current_stack_size_ = static_cast<int32_t>(stack_sizes_.at(next_addr).get_value());
        if (stack_sizes_.at(next_addr).is_processed()) {
            stack_sizes_[next_addr] = stack_size{from_initial_t, static_cast<uint32_t>(current_stack_size_)};
        }
    }

    void verifier::process_call_barray() {
        int32_t elements_size = pop_next_int32();
        validate(elements_size >= 0, "CALL_BARRAY: elements size must be non-negative. Bytecode offset: %#X\n");
    }

    uint32_t verifier::get_locals_size() const {
        return bytefile_.get_int32(current_frame_addr_ + sizeof(bytecode) + sizeof(int32_t));
    }

    void verifier::set_locals_size(uint32_t locals_size) {
        bytefile_.set_int32(current_frame_addr_ + sizeof(bytecode) + sizeof(int32_t), static_cast<int32_t>(locals_size));
    }

    uint32_t verifier::get_args_size() const {
        return bytefile_.get_int32(current_frame_addr_ + sizeof(bytecode));
    }

    void verifier::validate(bool condition, std::string_view message) const noexcept {
        if (!condition) {
            failure(const_cast<char*>(message.data()), addr_);
        }
    }

    void verify(bytefile& file) {
        constexpr static std::string_view ENTRYPOINT = "main";
        std::optional<public_symbol> entrypoint;
        uint32_t i = 0;
        while (!entrypoint && i < file.get_public_symbols_size()) {
            if (file.get_public_symbol_name(i) == ENTRYPOINT) {
                entrypoint = file.get_public_symbol(i);
            }
            ++i;
        }
        if (!entrypoint) {
            failure(const_cast<char*>("Entrypoint is not specified\n"));
        }
        verifier bytecode_verifier(file, *entrypoint);
        bytecode_verifier.traverse_bytecode();
    }

}
