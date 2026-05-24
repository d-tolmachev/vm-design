#ifndef BOUNDED_DEQUE_H
#define BOUNDED_DEQUE_H

#include <memory>
#include <utility>
#include <vector>

namespace assignment_10 {

    template <class T, class Container = std::vector<T>>
    class bounded_deque {
    public:
        explicit bounded_deque(size_t capacity);

        bounded_deque(const bounded_deque<T, Container>& other);

        bounded_deque(bounded_deque<T, Container>&& other) noexcept;

        bounded_deque<T, Container>& operator=(const bounded_deque<T, Container>& rhs);

        bounded_deque<T, Container>& operator=(bounded_deque<T, Container>&& rhs) noexcept;

        ~bounded_deque() = default;

        [[nodiscard]] const T& back() const;

        [[nodiscard]] T& back();

        [[nodiscard]] const T& front() const;

        [[nodiscard]] T& front();

        [[nodiscard]] bool empty() const noexcept;

        [[nodiscard]] size_t size() const noexcept;

        [[nodiscard]] size_t capacity() const noexcept;

        void push_back(const T& item);

        void push_back(T&& item);

        void pop_back();

        void push_front(const T& item);

        void push_front(T&& item);

        void pop_front();

        void swap(bounded_deque<T, Container>& other) noexcept;

    private:
        Container container_;
        size_t front_pos_;
        size_t back_pos_;
        size_t count_;
    };

    template <class T, class Container>
    bounded_deque<T, Container>::bounded_deque(size_t capacity)
        : container_(capacity)
        , front_pos_(!container_.empty() ? container_.size() / 2 : 0)
        , back_pos_(!container_.empty() ? (container_.size() / 2 + 1) % container_.size() : 0)
        , count_(0) {
    }

    template <class T, class Container>
    bounded_deque<T, Container>::bounded_deque(const bounded_deque<T, Container>& other)
        : container_(other.container_)
        , front_pos_(other.front_pos_)
        , back_pos_(other.back_pos_)
        , count_(other.count_) {
    }

    template <class T, class Container>
    bounded_deque<T, Container>::bounded_deque(bounded_deque<T, Container>&& other) noexcept
        : container_(std::exchange(other.container_, Container{other.container_.size()}))
        , front_pos_(std::exchange(other.front_pos_, !other.container_.empty() ? other.container_.size() / 2 : 0))
        , back_pos_(std::exchange(other.back_pos_, other.container_.empty() ? (other.container_.size() / 2 + 1) % other.container_.size() : 0))
        , count_(std::exchange(other.count_, 0)) {
    }

    template <class T, class Container>
    bounded_deque<T, Container>& bounded_deque<T, Container>::operator=(const bounded_deque<T, Container>& rhs) {
        if (std::addressof(rhs) != this) {
            bounded_deque<T, Container> tmp(rhs);
            swap(tmp);
        }
        return *this;
    }

    template <class T, class Container>
    bounded_deque<T, Container>& bounded_deque<T, Container>::operator=(bounded_deque<T, Container>&& rhs) noexcept {
        if (std::addressof(rhs) != this) {
            bounded_deque<T, Container> tmp(std::move(rhs));
            swap(tmp);
        }
        return *this;
    }

    template <class T, class Container>
    const T& bounded_deque<T, Container>::back() const {
        return container_.at((back_pos_ + container_.size() - 1) % container_.size());
    }

    template <class T, class Container>
    T& bounded_deque<T, Container>::back() {
        return container_.at((back_pos_ + container_.size() - 1) % container_.size());
    }

    template <class T, class Container>
    const T& bounded_deque<T, Container>::front() const {
        return container_.at((front_pos_ + 1) % container_.size());
    }

    template <class T, class Container>
    T& bounded_deque<T, Container>::front() {
        return container_.at((front_pos_ + 1) % container_.size());
    }

    template <class T, class Container>
    bool bounded_deque<T, Container>::empty() const noexcept {
        return count_ == 0;
    }

    template <class T, class Container>
    size_t bounded_deque<T, Container>::size() const noexcept {
        return count_;
    }

    template <class T, class Container>
    size_t bounded_deque<T, Container>::capacity() const noexcept {
        return container_.size();
    }

    template <class T, class Container>
    void bounded_deque<T, Container>::push_back(const T& item) {
        container_[back_pos_] = item;
        ++count_;
        back_pos_ = (back_pos_ + 1) % container_.size();
    }

    template <class T, class Container>
    void bounded_deque<T, Container>::push_back(T&& item) {
        container_[back_pos_] = std::move(item);
        ++count_;
        back_pos_ = (back_pos_ + 1) % container_.size();
    }

    template <class T, class Container>
    void bounded_deque<T, Container>::pop_back() {
        back_pos_ = (back_pos_ + container_.size() - 1) % container_.size();
        --count_;
    }

    template <class T, class Container>
    void bounded_deque<T, Container>::push_front(const T& item) {
        container_[front_pos_] = item;
        ++count_;
        front_pos_ = (front_pos_ + container_.size() - 1) % container_.size();
    }

    template <class T, class Container>
    void bounded_deque<T, Container>::push_front(T&& item) {
        container_[front_pos_] = std::move(item);
        ++count_;
        front_pos_ = (front_pos_ + container_.size() - 1) % container_.size();
    }

    template <class T, class Container>
    void bounded_deque<T, Container>::pop_front() {
        front_pos_ = (front_pos_ + 1) % container_.size();
        --count_;
    }

    template <class T, class Container>
    void bounded_deque<T, Container>::swap(bounded_deque<T, Container>& other) noexcept {
        std::swap(container_, other.container_);
        std::swap(front_pos_, other.front_pos_);
        std::swap(back_pos_, other.back_pos_);
        std::swap(count_, other.count_);
    }

}

#endif
