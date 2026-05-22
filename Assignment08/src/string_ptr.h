#ifndef STRING_PTR_H
#define STRING_PTR_H

#include <cstdint>
#include <cstdlib>
#include <iostream>
#include <ostream>
#include <string_view>

namespace assignment_08 {

    class string_ptr {
    public:
        string_ptr() noexcept;

        string_ptr(const char* str) noexcept;

        string_ptr(std::string_view str) noexcept;

        string_ptr(const string_ptr& other) noexcept;

        string_ptr(string_ptr&& other) noexcept;

        string_ptr& operator=(const string_ptr& rhs) noexcept;

        string_ptr& operator=(string_ptr&& rhs) noexcept;

        ~string_ptr();

        [[nodiscard]] bool unique() const noexcept;

        [[nodiscard]] char* data() const noexcept;

        operator std::string_view() const noexcept;

        void swap(string_ptr& other) noexcept;

    private:
        constexpr static uintptr_t UNIQUE = 1;
        constexpr static uintptr_t PTR = ~UNIQUE;
        mutable uintptr_t repr_;

        static uintptr_t to_repr(const char* ptr, bool unique) noexcept;
    };

    std::ostream& operator<<(std::ostream& os, const string_ptr& str);

    inline string_ptr::~string_ptr() {
        if (unique()) {
#ifdef STRING_PTR_DEBUG
            std::cerr << "Debug: deallocating memory for string: \"" << static_cast<std::string_view>(*this) << '\"' << std::endl;
#endif
            std::free(data());
        }
    }

}

#endif
