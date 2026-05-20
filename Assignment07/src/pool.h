#ifndef POOL_H
#define POOL_H

#include <atomic>
#include <cstddef>
#include <cstdlib>
#include <mutex>

namespace assignment_07 {

    struct standard_allocator_t {};

    struct synchronized_pool_t {};

    struct lock_free_pool_t {};

    struct thread_local_pool_t {};

    class pool_base {
    public:
        ~pool_base();

    protected:
        size_t total_size_;
        std::byte* base_;

        pool_base(size_t capacity, size_t max_chunk_size);

    private:
        size_t id_;

        static size_t next_pagesize(size_t size);
    };

    class synchronized_pool : public pool_base {
    public:
        synchronized_pool(size_t capacity, size_t max_chunk_size);

        inline void* allocate(size_t size);

        inline static synchronized_pool* get_or_create(size_t capacity, size_t max_chunk_size);

    private:
        std::byte* first_free_;
        std::mutex mutex_;
    };

    class lock_free_pool : public pool_base {
    public:
        lock_free_pool(size_t capacity, size_t max_chunk_size);

        inline void* allocate(size_t size);

        inline static lock_free_pool* get_or_create(size_t capacity, size_t max_chunk_size);

    private:
        std::atomic<std::byte*> first_free_;
    };

    class thread_local_pool : public pool_base {
    public:
        thread_local_pool(size_t capacity, size_t max_chunk_size);

        inline void* allocate(size_t size);

        inline static thread_local_pool* get_or_create(size_t capacity, size_t max_chunk_size);

    private:
        std::byte* first_free_;
    };

    inline void* synchronized_pool::allocate(size_t size) {
        std::scoped_lock<std::mutex> lock(mutex_);
        first_free_ -= size;
        return first_free_;
    }

    inline synchronized_pool* synchronized_pool::get_or_create(size_t capacity, size_t max_chunk_size) {
        static synchronized_pool pool(capacity, max_chunk_size);
        return &pool;
    }

    inline void* lock_free_pool::allocate(size_t size) {
        return first_free_.fetch_sub(static_cast<std::ptrdiff_t>(size), std::memory_order_relaxed) - size;
    }

    inline lock_free_pool* lock_free_pool::get_or_create(size_t capacity, size_t max_chunk_size) {
        static lock_free_pool pool(capacity, max_chunk_size);
        return &pool;
    }

    inline void* thread_local_pool::allocate(size_t size) {
        first_free_ -= size;
        return first_free_;
    }

    inline thread_local_pool* thread_local_pool::get_or_create(size_t capacity, size_t max_chunk_size) {
        static thread_local thread_local_pool pool(capacity, max_chunk_size);
        return &pool;
    }

}

inline void* operator new(size_t size, assignment_07::standard_allocator_t) {
    return std::malloc(size);
}

inline void* operator new(size_t size, assignment_07::synchronized_pool_t) {
    return assignment_07::synchronized_pool::get_or_create(0, 0)->allocate(size);
}

inline void* operator new(size_t size, assignment_07::lock_free_pool_t) {
    return assignment_07::lock_free_pool::get_or_create(0, 0)->allocate(size);
}

inline void* operator new(size_t size, assignment_07::thread_local_pool_t) {
    return assignment_07::thread_local_pool::get_or_create(0, 0)->allocate(size);
}

inline void operator delete(void* ptr, assignment_07::standard_allocator_t) noexcept {
    std::free(ptr);
}

inline void operator delete(void* ptr, assignment_07::synchronized_pool_t) noexcept {
}

inline void operator delete(void* ptr, assignment_07::lock_free_pool_t) noexcept {
}

inline void operator delete(void* ptr, assignment_07::thread_local_pool_t) noexcept {
}

#endif
