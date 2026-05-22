#include "string_ptr.h"

#include <cstring>
#include <memory>
#include <utility>

namespace assignment_08 {

    string_ptr::string_ptr() noexcept
        : repr_(to_repr(nullptr, false)) {
    }

    string_ptr::string_ptr(const char* str) noexcept
        : repr_(to_repr(str ? strdup(str) : nullptr, str != nullptr)) {
    }

    string_ptr::string_ptr(std::string_view str) noexcept
        : string_ptr(str.data()) {
    }

    string_ptr::string_ptr(const string_ptr& other) noexcept
        : repr_(other.repr_ & PTR) {
        other.repr_ = repr_;
    }

    string_ptr::string_ptr(string_ptr&& other) noexcept
        : repr_(std::exchange(other.repr_, to_repr(nullptr, false))) {
    }

    string_ptr& string_ptr::operator=(const string_ptr& rhs) noexcept {
        if (std::addressof(rhs) != this) {
            string_ptr tmp(rhs);
            swap(tmp);
        }
        return *this;
    }

    string_ptr& string_ptr::operator=(string_ptr&& rhs) noexcept {
        if (std::addressof(rhs) != this) {
            string_ptr tmp(std::move(rhs));
            swap(tmp);
        }
        return *this;
    }

    bool string_ptr::unique() const noexcept {
        return static_cast<bool>(repr_ & UNIQUE);
    }

    char* string_ptr::data() const noexcept {
        return reinterpret_cast<char*>(repr_ & PTR);
    }

    string_ptr::operator std::string_view() const noexcept {
        char* ptr = data();
        return std::string_view{ptr ? ptr : ""};
    }

    void string_ptr::swap(string_ptr& other) noexcept {
        std::swap(repr_, other.repr_);
    }

    uintptr_t string_ptr::to_repr(const char* ptr, bool unique) noexcept {
        return (reinterpret_cast<uintptr_t>(ptr) | unique);
    }

    std::ostream& operator<<(std::ostream& os, const string_ptr& str) {
        return os << (str.unique() ? "unique" : "non-unique") << " string: \"" << static_cast<std::string_view>(str) << '\"';
    }

}
