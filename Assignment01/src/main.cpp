#include <algorithm>
#include <chrono>
#include <cstdint>
#include <iostream>
#include <new>
#include <numeric>
#include <random>
#include <unistd.h>
#include <unordered_map>
#include <utility>
#include <vector>

namespace assignment_01 {

    class element {
    public:
        element* next;
        size_t data;

        element()
            : next(nullptr)
            , data(1) {
        }
    };

    double measure_stride(size_t size, size_t stride, size_t benches_cnt = 5) {
        const size_t ELEMENTS_CNT = size / sizeof(element);
        double duration = 0;
        for (size_t bench = 0; bench < benches_cnt; ++bench) {
            element* buf = new (std::align_val_t{static_cast<size_t>(sysconf(_SC_PAGE_SIZE))}) element[ELEMENTS_CNT];
            for (size_t i = 0; i < ELEMENTS_CNT; i += stride) {
                buf[i].data *= i;
            }
            std::fill(buf, buf + ELEMENTS_CNT, element{});
            std::chrono::high_resolution_clock::time_point start_time = std::chrono::high_resolution_clock::now();
            for (size_t i = 0; i < ELEMENTS_CNT; i += stride) {
                buf[i].data *= i;
            }
            std::chrono::high_resolution_clock::time_point end_time = std::chrono::high_resolution_clock::now();
            duration += static_cast<double>(std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time).count());
            delete[] buf;
        }
        return duration / static_cast<double>(benches_cnt);
    }

    double measure_traverse(size_t size, size_t benches_cnt = 1) {
        constexpr static size_t TRIES_CNT = 64;
        const size_t ELEMENTS_CNT = size / sizeof(element);
        std::random_device rd;
        std::mt19937_64 rng(rd());
        double duration = 0;
        for (size_t bench = 0; bench < benches_cnt; ++bench) {
            element* buf = new (std::align_val_t{static_cast<size_t>(sysconf(_SC_PAGE_SIZE))}) element[ELEMENTS_CNT];
            std::vector<size_t> indices(ELEMENTS_CNT);
            std::iota(indices.begin(), indices.end(), 0);
            std::shuffle(indices.begin(), indices.end(), rng);
            element* cur = buf;
            for (size_t idx : indices) {
                cur->next = buf + idx;
                cur = cur->next;
            }
            cur->next = buf;
            for (size_t i = 0; i < 4; ++i) {
                volatile element* it = buf;
                for (size_t j = 0; j < ELEMENTS_CNT; ++j) {
                    it = it->next;
                }
            }
            std::chrono::high_resolution_clock::time_point start_time = std::chrono::high_resolution_clock::now();
            for (size_t i = 0; i < TRIES_CNT; ++i) {
                volatile element* it = buf;
                for (size_t j = 0; j < ELEMENTS_CNT; ++j) {
                    it = it->next;
                }
            }
            std::chrono::high_resolution_clock::time_point end_time = std::chrono::high_resolution_clock::now();
            duration += static_cast<double>(std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time).count()) / static_cast<double>(ELEMENTS_CNT * TRIES_CNT);
            delete[] buf;
        }
        return duration / static_cast<double>(benches_cnt);
    }

    double measure_conflicts(size_t k, size_t stride, size_t benches_cnt = 10) {
        constexpr static size_t SIZE = 1 << 26;
        constexpr static size_t TRIES_CNT = 100;
        constexpr static size_t ELEMENTS_CNT = SIZE / sizeof(element);
        double duration = 0;
        for (size_t bench = 0; bench < benches_cnt; ++bench) {
            element* buf = new (std::align_val_t{static_cast<size_t>(sysconf(_SC_PAGE_SIZE))}) element[ELEMENTS_CNT];
            element* cur = buf;
            size_t cur_idx = 0;
            for (size_t i = 0; i < ELEMENTS_CNT; ++i) {
                cur_idx = ((cur_idx + stride) % ELEMENTS_CNT) + (cur_idx + stride >= ELEMENTS_CNT ? 1 : 0);
                cur->next = buf + cur_idx;
                cur = cur->next;
            }
            for (size_t i = 0; i < TRIES_CNT; ++i) {
                volatile element* it = buf;
                for (size_t j = 0; j < k; ++j) {
                    it = it->next;
                }
            }
            std::chrono::high_resolution_clock::time_point start_time = std::chrono::high_resolution_clock::now();
            for (size_t i = 0; i < TRIES_CNT; ++i) {
                volatile element* it = buf;
                for (size_t j = 0; j < k; ++j) {
                    it = it->next;
                }
            }
            std::chrono::high_resolution_clock::time_point end_time = std::chrono::high_resolution_clock::now();
            duration += static_cast<double>(std::chrono::duration_cast<std::chrono::nanoseconds>(end_time - start_time).count()) / static_cast<double>(k * TRIES_CNT);
            delete[] buf;
        }
        return duration / static_cast<double>(benches_cnt);
    }

}

int main() {
    constexpr static double LINE_THRESHOLD = 1.4;
    constexpr static double SIZE_THRESHOLD = 1.4;
    constexpr static double ASSOC_THRESHOLD = 1.5;
    size_t size_min = 1 << 10;
    size_t size_max = 1 << 28;
    size_t detected_line = 1;
    size_t prev_stride = 1;
    double prev_time = 0;
    size_t cur_stride = 1;
    while (cur_stride <= 1024) {
        double cur_time = assignment_01::measure_stride(size_max, cur_stride);
        if (cur_stride == 1) {
            prev_time = cur_time;
        } else {
            if (cur_time * LINE_THRESHOLD < prev_time) {
                detected_line = prev_stride;
                break;
            }
            prev_stride = cur_stride;
            prev_time = cur_time;
        }
        cur_stride += cur_stride >= 8 ? cur_stride : 1;
    }
    std::cout << "Line size == " << (detected_line * sizeof(size_t)) << " bytes" << std::endl;
    size_max = 1 << 21;
    size_t benches_max = 5;
    size_t detected_l1 = 0;
    size_t cnt = 0;
    prev_time = 0;
    while (detected_l1 == 0) {
        std::vector<size_t> tries;
        for (size_t bench = 0; bench < benches_max; ++bench) {
            std::vector<std::pair<size_t, double>> measures;
            for (size_t size = size_min; size <= size_max; size *= 2) {
                for (size_t step = 0; step < size; step += size / 2) {
                    double time = assignment_01::measure_traverse(size + step, 32);
                    measures.emplace_back(size + step, time);
                }
            }
            for (size_t i = 1; i < measures.size(); ++i) {
                prev_time = measures.at(i - 1).second;
                double cur_time = measures.at(i).second;
                if (i > 3 && cur_time > prev_time * SIZE_THRESHOLD) {
                    tries.push_back(measures.at(i - 1).first);
                    break;
                }
            }
        }
        ++cnt;
        if (tries.empty()) {
            if (cnt >= 5) {
                detected_l1 = 0;
                break;
            }
            continue;
        }
        if (tries.size() > benches_max / 2) {
            auto second_projection = [](const std::pair<size_t, size_t>& lhs, const std::pair<size_t, size_t>& rhs) {
                return lhs.second < rhs.second;
            };
            std::unordered_map<size_t, size_t> frequency;
            for (size_t size : tries) {
                ++frequency[size];
            }
            detected_l1 = std::max_element(frequency.begin(), frequency.end(), second_projection)->first;
        }
    }
    std::cout << "L1 size == " << (detected_l1 / 1024) << " KB" << std::endl;
    size_t detected_assoc = 1;
    size_t prev_k = 1;
    prev_time = 0;
    size_t cur_k = 1;
    while (detected_l1 >= 1024 && cur_k <= 32) {
        double cur_time = assignment_01::measure_conflicts(cur_k, detected_l1);
        if (cur_k > 1 && cur_time > prev_time * ASSOC_THRESHOLD) {
            detected_assoc = prev_k;
            break;
        }
        detected_assoc = cur_k;
        prev_k = cur_k;
        prev_time = cur_time;
        cur_k += cur_k >= 8 ? 4 : 1;
    }
    std::cout << "Associativity == " << detected_assoc << std::endl;
    return 0;
}
