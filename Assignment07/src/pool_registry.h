#ifndef POOL_REGISTRY_H
#define POOL_REGISTRY_H

#include <array>
#include <atomic>
#include <cstddef>

namespace assignment_07 {

    class pool_registry {
    public:
        constexpr static size_t npos = static_cast<size_t>(-1);

        pool_registry();

        size_t register_pool(std::byte* guard_first, std::byte* guard_last);

        void unregister_pool(size_t pool_id);

        size_t find_pool(const std::byte* addr) const;

        static pool_registry* get_or_create();

    private:
        constexpr static size_t MAX_POOLS = 0xFF;
        std::array<std::atomic<std::byte*>, MAX_POOLS> guard_firsts_;
        std::array<std::atomic<std::byte*>, MAX_POOLS> guard_lasts_;
    };

}

#endif