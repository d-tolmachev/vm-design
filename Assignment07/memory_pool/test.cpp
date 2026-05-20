#include <cstdint>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <sys/resource.h>
#include <sys/time.h>
#include <thread>
#include <utility>
#include <vector>

#include "pool.h"

struct Node {
    Node* next;
    unsigned node_id;
};

static void get_usage(rusage& usage) {
    if (getrusage(RUSAGE_SELF, &usage)) {
        std::perror("Cannot get usage");
        std::exit(EXIT_SUCCESS);
    }
}

template <class Allocator>
static inline Node* create_list(unsigned n) {
    Node* list = nullptr;
    for (unsigned i = 0; i < n; ++i) {
        list = new (Allocator{}) Node(list, i);
    }
    return list;
}

template <class Allocator>
static inline void delete_list(Node* list) {
    while (list) {
        Node* node = list;
        list = list->next;
        operator delete(node, Allocator{});
    }
}

template <class Allocator, bool ThreadLocal = false, class Initializer>
static inline void test(unsigned threads_cnt, unsigned n, Initializer&& initializer) {
    auto testcase = [n, initializer = std::forward<Initializer>(initializer)]() {
        if constexpr (ThreadLocal) {
            initializer();
        }
        delete_list<Allocator>(create_list<Allocator>(n));
    };
    rusage start;
    rusage finish;
    get_usage(start);
    if constexpr (!ThreadLocal) {
        initializer();
    }
    std::vector<std::thread> workers;
    workers.reserve(threads_cnt);
    for (unsigned i = 0; i < threads_cnt; ++i) {
        workers.emplace_back(testcase);
    }
    for (std::thread& worker : workers) {
        worker.join();
    }
    get_usage(finish);
    timeval diff;
    timersub(&finish.ru_utime, &start.ru_utime, &diff);
    int64_t time_used = diff.tv_sec * 1000000 + diff.tv_usec;
    std::cout << "Time used: " << time_used << " usec" << std::endl;
    int64_t mem_used = (finish.ru_maxrss - start.ru_maxrss) * 1024;
    std::cout << "Memory used: " << mem_used << " bytes" << std::endl;
    int64_t mem_required = static_cast<int64_t>(threads_cnt * n * sizeof(Node));
    double overhead = (mem_used - mem_required) * static_cast<double>(100) / mem_used;
    std::cout << "Overhead: " << std::fixed << std::setw(4) << std::setprecision(1) << overhead << '%' << std::endl;
}

int main(const int argc, const char** argv) {
    constexpr static unsigned THREADS_CNT = 16;
    constexpr static unsigned N = 10000000;
    std::cout << "Standard allocator:" << std::endl;
    test<assignment_07::standard_allocator_t>(THREADS_CNT, N, []() {});
    std::cout << "Global synchronized pool:" << std::endl;
    test<assignment_07::synchronized_pool_t>(THREADS_CNT, N, []() { assignment_07::synchronized_pool::get_or_create(THREADS_CNT * N * sizeof(Node), sizeof(Node)); });
    std::cout << "Global lock-free pool:" << std::endl;
    test<assignment_07::lock_free_pool_t>(THREADS_CNT, N, []() { assignment_07::lock_free_pool::get_or_create(THREADS_CNT * N * sizeof(Node), sizeof(Node)); });
    std::cout << "Thread-local pools:" << std::endl;
    test<assignment_07::thread_local_pool_t, true>(THREADS_CNT, N, []() { assignment_07::thread_local_pool::get_or_create(N * sizeof(Node), sizeof(Node)); });
    return EXIT_SUCCESS;
}
