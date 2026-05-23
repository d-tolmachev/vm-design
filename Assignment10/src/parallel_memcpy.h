#ifndef PARALLEL_MEMCPY_H
#define PARALLEL_MEMCPY_H

#include <cstddef>

namespace assignment_10 {

    void initialize_thread_pool(size_t threads_cnt);

    void* parallel_memcpy(void* dst, const void* src, size_t size);

}

#endif
