#include <cassert>
#include <chrono>
#include <cstddef>
#include <cstdint>
#include <iostream>
#include <limits>
#include <random>
#include <string>
#include <vector>

#include "parallel_memcpy.h"

template <class TestCase>
void test(const std::string& test_name, TestCase& test_case) {
    std::cout << test_name << " started" << std::endl;
    test_case();
    std::cout << test_name << " finished" << std::endl << std::endl << std::endl;
}

void test_parallel_memcpy(size_t threads_cnt, size_t size) {
    static std::mt19937 rng(std::random_device{}());
    assignment_10::initialize_thread_pool(threads_cnt);
    std::vector<std::byte> src(size);
    std::vector<std::byte> dst(size);
    std::uniform_int_distribution<uint32_t> dist(static_cast<uint32_t>(std::numeric_limits<std::byte>::min()), static_cast<uint32_t>(std::numeric_limits<std::byte>::max()));
    for (std::byte& item : src) {
        item = static_cast<std::byte>(dist(rng));
    }
    std::chrono::high_resolution_clock::time_point start_time = std::chrono::high_resolution_clock::now();
    assignment_10::parallel_memcpy(dst.data(), src.data(), size);
    std::chrono::high_resolution_clock::time_point end_time = std::chrono::high_resolution_clock::now();
    size_t duration = std::chrono::duration_cast<std::chrono::microseconds>(end_time - start_time).count();
    std::cout << "Time used: " << duration << " ms" << std::endl;
    assert(src == dst);
}

int main() {
    constexpr static size_t THREADS_CNT = 8;
    constexpr static size_t SIZE = 1 << 28;
    for (size_t i = 0; i <= THREADS_CNT; ++i) {
        std::string test_name("Pool with " + std::to_string(i) + " threads test");
        auto test_func = [i]() {
            test_parallel_memcpy(i, SIZE);
        };
        test(test_name, test_func);
    }
    return 0;
}
