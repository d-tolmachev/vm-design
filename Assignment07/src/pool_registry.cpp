#include "pool_registry.h"

#include <cstdlib>
#include <cstdio>

#include "sigsegv_handler.h"

namespace assignment_07 {

    pool_registry::pool_registry() {
        register_sigsegv_handler();
    }

    size_t pool_registry::register_pool(std::byte* guard_first, std::byte* guard_last) {
        for (size_t i = 0; i < guard_firsts_.size(); ++i) {
            std::byte* expected = nullptr;
            if (guard_firsts_.at(i).compare_exchange_strong(expected, guard_first, std::memory_order_release, std::memory_order_relaxed)) {
                guard_lasts_[i].store(guard_last, std::memory_order_relaxed);
                return i;
            }
        }
        std::perror("too many pools");
        std::exit(EXIT_FAILURE);
    }

    void pool_registry::unregister_pool(size_t pool_id) {
        guard_firsts_[pool_id].store(nullptr, std::memory_order_release);
    }

    size_t pool_registry::find_pool(const std::byte* addr) const {
        for (size_t i = 0; i < guard_firsts_.size(); ++i) {
            std::byte* guard_first = guard_firsts_.at(i).load(std::memory_order_acquire);
            if (guard_first) {
                std::byte* guard_last_ = guard_lasts_.at(i).load(std::memory_order_acquire);
                if (addr >= guard_first && addr < guard_last_) {
                    return i;
                }
            }
        }
        return npos;
    }

    pool_registry* pool_registry::get_or_create() {
        static pool_registry registry;
        return &registry;
    }

}
