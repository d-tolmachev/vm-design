#ifndef STACK_H
#define STACK_H

#include <array>
#include <iterator>
#include <new>
#include <type_traits>
#include <utility>

#include "runtime_interface.h"

namespace assignment_04 {

    constexpr inline size_t MAX_STACK_SIZE = 0xFFFFF;
    inline std::array<auint, MAX_STACK_SIZE> stack_buf{};

    template <class T>
    class stack {
    public:
        using element_type = T;
        using value_type = std::remove_cv_t<T>;
        using difference_type = std::ptrdiff_t;
        using pointer = T*;
        using const_pointer = const T*;
        using reference = T&;
        using const_reference = const T&;
        using iterator = T*;
        using const_iterator = const iterator;
        using reverse_iterator = std::reverse_iterator<iterator>;
        using const_reverse_iterator = const reverse_iterator;

        constexpr stack() = default;

        constexpr stack(pointer first, size_t count) noexcept;

        constexpr stack(pointer first, pointer last);

        constexpr stack(const stack<T>& other) noexcept = default;

        constexpr stack(stack<T>&& other) noexcept = default;

        constexpr stack<T>& operator=(const stack<T>& rhs) noexcept = default;

        constexpr stack<T>& operator=(stack<T>&& rhs) noexcept = default;

        [[nodiscard]] constexpr bool empty() const noexcept;

        [[nodiscard]] constexpr size_t size() const noexcept;

        [[nodiscard]] constexpr const typename stack<T>::pointer data() const noexcept;

        [[nodiscard]] constexpr typename stack<T>::pointer data() noexcept;

        [[nodiscard]] constexpr typename stack<T>::const_iterator cbegin() const noexcept;

        [[nodiscard]] constexpr typename stack<T>::const_iterator begin() const noexcept;

        [[nodiscard]] constexpr typename stack<T>::iterator begin() noexcept;

        [[nodiscard]] constexpr typename stack<T>::const_iterator cend() const noexcept;

        [[nodiscard]] constexpr typename stack<T>::const_iterator end() const noexcept;

        [[nodiscard]] constexpr typename stack<T>::iterator end() noexcept;

        [[nodiscard]] constexpr typename stack<T>::const_reverse_iterator crbegin() const noexcept;

        [[nodiscard]] constexpr typename stack<T>::const_reverse_iterator rbegin() const noexcept;

        [[nodiscard]] constexpr typename stack<T>::reverse_iterator rbegin() noexcept;

        [[nodiscard]] constexpr typename stack<T>::const_reverse_iterator crend() const noexcept;

        [[nodiscard]] constexpr typename stack<T>::const_reverse_iterator rend() const noexcept;

        [[nodiscard]] constexpr typename stack<T>::reverse_iterator rend() noexcept;

        [[nodiscard]] constexpr const typename stack<T>::reference top(size_t offset = 0) const noexcept;

        [[nodiscard]] constexpr typename stack<T>::reference top(size_t offset = 0) noexcept;

        constexpr void push(const element_type& value);

        constexpr void push(element_type&& value) noexcept;

        constexpr typename stack<T>::element_type pop() noexcept;

        constexpr void pop(size_t count) noexcept;

        constexpr const typename stack<T>::reference operator[](size_t idx) const noexcept;

        constexpr typename stack<T>::reference operator[](size_t idx) noexcept;
    };

    template <class T>
    constexpr stack<T>::stack(pointer first, size_t count) noexcept {
        __gc_stack_top = reinterpret_cast<size_t>(first);
        __gc_stack_bottom = __gc_stack_top + count * sizeof(element_type);
    }

    template <class T>
    constexpr stack<T>::stack(pointer first, pointer last) {
        __gc_stack_top = reinterpret_cast<size_t>(first);
        __gc_stack_bottom = reinterpret_cast<size_t>(last);
    }

    template <class T>
    constexpr bool stack<T>::empty() const noexcept {
        return size() == 0;
    }

    template <class T>
    constexpr size_t stack<T>::size() const noexcept {
        return (__gc_stack_bottom - __gc_stack_top) / sizeof(element_type);
    }

    template <class T>
    constexpr const typename stack<T>::pointer stack<T>::data() const noexcept {
        return std::launder(reinterpret_cast<const pointer>(__gc_stack_top));
    }

    template <class T>
    constexpr typename stack<T>::pointer stack<T>::data() noexcept {
        return std::launder(reinterpret_cast<pointer>(__gc_stack_top));
    }

    template <class T>
    constexpr typename stack<T>::const_iterator stack<T>::cbegin() const noexcept {
        return std::launder(reinterpret_cast<const_iterator>(__gc_stack_top));
    }

    template <class T>
    constexpr typename stack<T>::const_iterator stack<T>::begin() const noexcept {
        return std::launder(reinterpret_cast<const_iterator>(__gc_stack_top));
    }

    template <class T>
    constexpr typename stack<T>::iterator stack<T>::begin() noexcept {
        return std::launder(reinterpret_cast<iterator>(__gc_stack_top));
    }

    template <class T>
    constexpr typename stack<T>::const_iterator stack<T>::cend() const noexcept {
        return std::launder(reinterpret_cast<const_iterator>(__gc_stack_bottom));
    }

    template <class T>
    constexpr typename stack<T>::const_iterator stack<T>::end() const noexcept {
        return std::launder(reinterpret_cast<const_iterator>(__gc_stack_bottom));
    }

    template <class T>
    constexpr typename stack<T>::iterator stack<T>::end() noexcept {
        return std::launder(reinterpret_cast<iterator>(__gc_stack_bottom));
    }

    template <class T>
    constexpr typename stack<T>::const_reverse_iterator stack<T>::crbegin() const noexcept {
        return reverse_iterator{cend()};
    }

    template <class T>
    constexpr typename stack<T>::const_reverse_iterator stack<T>::rbegin() const noexcept {
        return reverse_iterator{end()};
    }

    template <class T>
    constexpr typename stack<T>::reverse_iterator stack<T>::rbegin() noexcept {
        return reverse_iterator{end()};
    }

    template <class T>
    constexpr typename stack<T>::const_reverse_iterator stack<T>::crend() const noexcept {
        return reverse_iterator{cbegin()};
    }

    template <class T>
    constexpr typename stack<T>::const_reverse_iterator stack<T>::rend() const noexcept {
        return reverse_iterator{begin()};
    }

    template <class T>
    constexpr typename stack<T>::reverse_iterator stack<T>::rend() noexcept {
        return reverse_iterator{begin()};
    }

    template <class T>
    constexpr const typename stack<T>::reference stack<T>::top(size_t offset) const noexcept {
        return (*this)[size() - offset - 1];
    }

    template <class T>
    constexpr typename stack<T>::reference stack<T>::top(size_t offset) noexcept {
        return (*this)[size() - offset - 1];
    }

    template <class T>
    constexpr void stack<T>::push(const element_type& value) {
        __gc_stack_bottom += sizeof(element_type);
        (*this)[size() - 1] = value;
    }

    template <class T>
    constexpr void stack<T>::push(element_type&& value) noexcept {
        __gc_stack_bottom += sizeof(element_type);
        (*this)[size() - 1] = std::move(value);
    }

    template <class T>
    constexpr typename stack<T>::element_type stack<T>::pop() noexcept {
        element_type value = top();
        __gc_stack_bottom -= sizeof(element_type);
        return value;
    }

    template <class T>
    constexpr void stack<T>::pop(size_t count) noexcept {
        __gc_stack_bottom -= count * sizeof(element_type);
    }

    template <class T>
    constexpr const typename stack<T>::reference stack<T>::operator[](size_t idx) const noexcept {
        return std::launder(reinterpret_cast<const pointer>(__gc_stack_top))[idx];
    }

    template <class T>
    constexpr typename stack<T>::reference stack<T>::operator[](size_t idx) noexcept {
        return std::launder(reinterpret_cast<pointer>(__gc_stack_top))[idx];
    }

}

#endif
