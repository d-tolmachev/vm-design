#ifndef BYTEFILE_H
#define BYTEFILE_H

#include <cstdint>
#include <string_view>
#include <vector>

namespace assignment_02 {

    enum class bytecode : uint8_t {
        BINOP_HIGH = 0x00,
        LOW_ADD = 0x01,
        LOW_SUB = 0x02,
        LOW_MUL = 0x03,
        LOW_DIV = 0x04,
        LOW_MOD = 0x05,
        LOW_LT = 0x06,
        LOW_LE = 0x07,
        LOW_GT = 0x08,
        LOW_GE = 0x09,
        LOW_EQ = 0x0A,
        LOW_NE = 0x0B,
        LOW_AND = 0x0C,
        LOW_OR = 0x0D,
        CONST = 0x10,
        STRING = 0x11,
        SEXP = 0x12,
        STI = 0x13,
        STA = 0x14,
        JMP = 0x15,
        END = 0x16,
        RET = 0x17,
        DROP = 0x18,
        DUP = 0x19,
        SWAP = 0x1A,
        ELEM = 0x1B,
        LD_GLOBAL = 0x20,
        LD_LOCAL = 0x21,
        LD_ARGUMENT = 0x22,
        LD_CAPTURED = 0x23,
        LDA_GLOBAL = 0x30,
        LDA_LOCAL = 0x31,
        LDA_ARGUMENT = 0x32,
        LDA_CAPTURED = 0x33,
        ST_GLOBAL = 0x40,
        ST_LOCAL = 0x41,
        ST_ARGUMENT = 0x42,
        ST_CAPTURED = 0x43,
        CJMPZ = 0x50,
        CJMPNZ = 0x51,
        BEGIN = 0x52,
        CBEGIN = 0x53,
        CLOSURE = 0x54,
        CALLC = 0x55,
        CALL = 0x56,
        TAG = 0x57,
        ARRAY = 0x58,
        FAIL = 0x59,
        LINE = 0x5A,
        PATT_STR = 0x60,
        PATT_STRING = 0x61,
        PATT_ARRAY = 0x62,
        PATT_SEXP = 0x63,
        PATT_REF = 0x64,
        PATT_VAL = 0x65,
        PATT_FUN = 0x66,
        CALL_LREAD = 0x70,
        CALL_LWRITE = 0x71,
        CALL_LLENGTH = 0x72,
        CALL_LSTRING = 0x73,
        CALL_BARRAY = 0x74,
        STOP = 0xF0
    };

    class public_symbol {
    public:
        public_symbol(size_t offset, uint32_t address, uint32_t name);

        [[nodiscard]] size_t get_offset() const;

        [[nodiscard]] uint32_t get_address() const;

        [[nodiscard]] uint32_t get_name() const;

    private:
        size_t offset_;
        uint32_t address_;
        uint32_t name_;
    };

    class bytefile {
    public:
        explicit bytefile(std::string_view name);

        [[nodiscard]] std::string_view get_name() const noexcept;

        [[nodiscard]] uint32_t get_global_area_size() const noexcept;

        void set_global_area_size(uint32_t global_area_size) noexcept;

        [[nodiscard]] uint32_t get_public_symbols_size() const noexcept;

        public_symbol get_public_symbol(uint32_t pos) const;

        void add_public_symbol(public_symbol symbol);

        [[nodiscard]] uint32_t get_string_tab_size() const noexcept;

        std::string_view get_string(uint32_t pos) const;

        void add_string(const std::vector<char>& string);

        std::string_view get_public_symbol_name(uint32_t pos) const;

        [[nodiscard]] uint32_t get_code_pos() const noexcept;

        void set_code_pos(uint32_t code_pos) noexcept;

        [[nodiscard]] uint32_t get_code_size() const noexcept;

        bytecode get_code(uint32_t pos) const;

        void add_code(const std::vector<bytecode>& code);

        int8_t get_int8(uint32_t pos) const;

        int32_t get_int32(uint32_t pos) const;

    private:
        std::string_view name_;
        uint32_t global_area_size_;
        std::vector<public_symbol> public_symbols_;
        std::vector<char> string_tab_;
        uint32_t code_pos_;
        std::vector<bytecode> code_;
    };

}

#endif
