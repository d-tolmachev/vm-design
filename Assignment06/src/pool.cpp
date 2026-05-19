#include "pool.h"

#include <cstdio>
#include <sys/mman.h>
#include <unistd.h>

namespace assignment_06 {

    pool::pool(size_t capacity, size_t max_chunk_size)
        : guard_size_(next_pagesize(max_chunk_size))
        , total_size_(next_pagesize(capacity) + guard_size_)
        , base_(static_cast<std::byte*>(mmap(nullptr, total_size_, PROT_READ | PROT_WRITE, MAP_PRIVATE | MAP_ANONYMOUS, 0, 0)))
        , first_free_(base_ + total_size_) {
        if (base_ == MAP_FAILED) {
            std::perror("mmap failed");
            std::exit(EXIT_FAILURE);
        }
        if (mprotect(base_, guard_size_, PROT_NONE) == -1) {
            std::perror("mprotect failed");
            std::exit(EXIT_FAILURE);
        }
    }

    pool::~pool() {
        munmap(base_, total_size_);
    }

    size_t pool::next_pagesize(size_t size) {
        static size_t pagesize = sysconf(_SC_PAGESIZE);
        return ((size + pagesize - 1) / pagesize + 1 / (size + 1)) * pagesize;
    }

}
