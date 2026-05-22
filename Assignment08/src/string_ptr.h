#ifndef STRING_PTR_H
#define STRING_PTR_H

#include <algorithm>
#include <cstdint>
#include <cstdlib>
#include <cstring>
#include <iostream>
#include <ostream>
#include <string_view>

namespace assignment_08 {

    class string_ptr {
    public:
        string_ptr() noexcept;

        string_ptr(const char* str);

        string_ptr(std::string_view str);

        string_ptr(const string_ptr& other) noexcept;

        string_ptr(string_ptr&& other) noexcept;

        string_ptr& operator=(const string_ptr& rhs) noexcept;

        string_ptr& operator=(string_ptr&& rhs) noexcept;

        ~string_ptr();

        [[nodiscard]] constexpr bool unique() const noexcept;

        [[nodiscard]] char* data() const noexcept;

        operator std::string_view() const noexcept;

        constexpr void swap(string_ptr& other) noexcept;

    private:
        constexpr static uintptr_t UNIQUE = 1;
        constexpr static uintptr_t PTR = ~UNIQUE;
        mutable uintptr_t repr_;

        static uintptr_t to_repr(const char* ptr, bool unique) noexcept;

        static char* allocate(const char* str);
    };

    inline string_ptr::string_ptr() noexcept
        : repr_(to_repr(nullptr, false)) {
    }

    inline string_ptr::string_ptr(const char* str)
        : repr_(to_repr(str ? allocate(str) : nullptr, str != nullptr)) {
    }

    inline string_ptr::string_ptr(std::string_view str)
        : string_ptr(str.data()) {
    }

    inline string_ptr::~string_ptr() {
        if (unique()) {
#ifdef STRING_PTR_DEBUG
            std::cerr << "Debug: deallocating memory for string: \"" << static_cast<std::string_view>(*this) << '\"' << std::endl;
#endif
            std::free(data());
        }
    }

    constexpr inline bool string_ptr::unique() const noexcept {
        return static_cast<bool>(repr_ & UNIQUE);
    }

    inline char* string_ptr::data() const noexcept {
        return reinterpret_cast<char*>(repr_ & PTR);
    }

    constexpr inline void string_ptr::swap(string_ptr& other) noexcept {
        std::swap(repr_, other.repr_);
    }

    inline uintptr_t string_ptr::to_repr(const char* ptr, bool unique) noexcept {
        return (reinterpret_cast<uintptr_t>(ptr) | unique);
    }

    inline char* string_ptr::allocate(const char* str) {
        size_t length = std::strlen(str);
        char* ptr = new char[length + 1];
        std::copy(str, str + length + 1, ptr);
        return ptr;
    }

    inline std::ostream& operator<<(std::ostream& os, const string_ptr& str) {
        return os << (str.unique() ? "unique" : "non-unique") << " string: \"" << static_cast<std::string_view>(str) << '\"';
    }

}

#endif
