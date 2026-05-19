#ifndef POOL_H
#define POOL_H

#include <cstddef>
#include <cstdlib>

namespace assignment_06 {

    struct standard_allocator {};

    struct custom_pool {};

    class pool {
    public:
        pool(size_t capacity, size_t max_chunk_size);

        ~pool();

        inline void* allocate(size_t size);

    private:
        size_t guard_size_;
        size_t total_size_;
        std::byte* base_;
        std::byte* first_free_;

        static size_t next_pagesize(size_t size);
    };

    inline void* pool::allocate(size_t size) {
        first_free_ -= size;
        return first_free_;
    }

    inline pool* get_pool(size_t capacity, size_t max_chunk_size) {
        static pool p(capacity, max_chunk_size);
        return &p;
    }

}

inline void* operator new(size_t size, assignment_06::standard_allocator) {
    return std::malloc(size);
}

inline void* operator new(size_t size, assignment_06::custom_pool) {
    assignment_06::pool* pool_ptr = assignment_06::get_pool(0, 0);
    return pool_ptr->allocate(size);
}

inline void operator delete(void* ptr, assignment_06::standard_allocator) noexcept {
    std::free(ptr);
}

inline void operator delete(void* ptr, assignment_06::custom_pool) {
}

#endif
