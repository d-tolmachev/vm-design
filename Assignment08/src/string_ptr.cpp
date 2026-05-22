#include "string_ptr.h"

#include <memory>
#include <utility>

namespace assignment_08 {

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

    string_ptr::operator std::string_view() const noexcept {
        char* ptr = data();
        return std::string_view{ptr ? ptr : ""};
    }

}
