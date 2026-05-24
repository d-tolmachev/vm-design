#include "parallel_memcpy.h"

#include <algorithm>
#include <condition_variable>
#include <cstddef>
#include <functional>
#include <mutex>
#include <optional>
#include <unistd.h>

#include "thread_pool.h"

namespace assignment_10 {

    static std::optional<thread_pool<std::function<void()>>> pool = std::nullopt;
    static size_t current_threads_cnt = 0;
    static std::mutex mutex;
    static std::condition_variable cond;
    static size_t remaining_tasks_cnt = 0;

    static void memcpy_chunk(std::byte* dst, const std::byte* src, size_t size) {
        std::copy_n(src, size, dst);
        {
            std::scoped_lock<std::mutex> lock(mutex);
            --remaining_tasks_cnt;
        }
        cond.notify_one();
    }

    void initialize_thread_pool(size_t threads_cnt) {
        pool.emplace(threads_cnt, threads_cnt);
        current_threads_cnt = threads_cnt;
    }

    void* parallel_memcpy(void* dst, const void* src, size_t size) {
        static size_t pagesize = sysconf(_SC_PAGESIZE);
        size_t tasks_cnt = std::clamp(size / pagesize - 1, static_cast<size_t>(0), current_threads_cnt);
        remaining_tasks_cnt = tasks_cnt;
        size_t chunk_size = size / (tasks_cnt + 1);
        size_t tail_offset = chunk_size * tasks_cnt;
        size_t tail_size = size - tail_offset;
        for (size_t i = 0; i < tasks_cnt; ++i) {
            auto copy_chunk = [dst, src, chunk_size, i]() {
                memcpy_chunk(static_cast<std::byte*>(dst) + i * chunk_size, static_cast<const std::byte*>(src) + i * chunk_size, chunk_size);
            };
            pool->submit(copy_chunk);
        }
        std::copy_n(static_cast<const std::byte*>(src) + tail_offset, tail_size, static_cast<std::byte*>(dst) + tail_offset);
        std::unique_lock<std::mutex> lock(mutex);
        while (remaining_tasks_cnt > 0) {
            cond.wait(lock);
        }
        return dst;
    }

}
