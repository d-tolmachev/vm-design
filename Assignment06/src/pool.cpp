#include "pool.h"

#include <cstdio>
#include <sys/mman.h>
#include <unistd.h>

#include "sigsegv_handler.h"

namespace assignment_06 {

    pool_base::pool_base(size_t capacity, size_t max_chunk_size)
        : total_size_(next_pagesize(capacity) + next_pagesize(max_chunk_size))
        , base_(static_cast<std::byte*>(mmap(nullptr, total_size_, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, 0, 0))) {
        size_t guard_size = next_pagesize(max_chunk_size);
        if (base_ == MAP_FAILED) {
            std::perror("mmap failed");
            std::exit(EXIT_FAILURE);
        }
        if (guard_size > 0 && mprotect(base_, guard_size, PROT_NONE) == -1) {
            std::perror("mprotect failed");
            std::exit(EXIT_FAILURE);
        }
        register_sigsegv_handler();
    }

    pool_base::~pool_base() {
        munmap(base_, total_size_);
    }

    size_t pool_base::next_pagesize(size_t size) {
        static size_t pagesize = sysconf(_SC_PAGESIZE);
        return (size + pagesize - 1) & ~(pagesize - 1);
    }

    pool::pool(size_t capacity, size_t max_chunk_size)
        : pool_base(capacity, max_chunk_size)
        , first_free_(base_ + total_size_) {
    }

}
