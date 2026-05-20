#ifndef POOL_H
#define POOL_H

#include <cstddef>
#include <cstdlib>

namespace assignment_06 {

    struct standard_allocator_t {};

    struct pool_t {};

    class pool_base {
    public:
        ~pool_base();

    protected:
        size_t total_size_;
        std::byte* base_;

        pool_base(size_t capacity, size_t max_chunk_size);

    private:
        static size_t next_pagesize(size_t size);
    };

    class pool : public pool_base {
    public:
        pool(size_t capacity, size_t max_chunk_size);

        inline void* allocate(size_t size);

        inline static pool* get_or_create(size_t capacity, size_t max_chunk_size);

    private:
        std::byte* first_free_;
    };

    inline void* pool::allocate(size_t size) {
        first_free_ -= size;
        return first_free_;
    }

    inline pool* pool::get_or_create(size_t capacity, size_t max_chunk_size) {
        static pool pool_instance(capacity, max_chunk_size);
        return &pool_instance;
    }

}

inline void* operator new(size_t size, assignment_06::standard_allocator_t) {
    return std::malloc(size);
}

inline void* operator new(size_t size, assignment_06::pool_t) {
    return assignment_06::pool::get_or_create(0, 0)->allocate(size);
}

inline void operator delete(void* ptr, assignment_06::standard_allocator_t) noexcept {
    std::free(ptr);
}

inline void operator delete(void* ptr, assignment_06::pool_t) noexcept {
}

#endif
