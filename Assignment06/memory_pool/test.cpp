#include <cstdint>
#include <cstdlib>
#include <iomanip>
#include <iostream>
#include <sys/resource.h>
#include <sys/time.h>

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

template <class Allocator>
static inline void test(unsigned n) {
    rusage start;
    rusage finish;
    get_usage(start);
    delete_list<Allocator>(create_list<Allocator>(n));
    get_usage(finish);
    timeval diff;
    timersub(&finish.ru_utime, &start.ru_utime, &diff);
    int64_t time_used = diff.tv_sec * 1000000 + diff.tv_usec;
    std::cout << "Time used: " << time_used << " usec" << std::endl;
    int64_t mem_used = (finish.ru_maxrss - start.ru_maxrss) * 1024;
    std::cout << "Memory used: " << mem_used << " bytes" << std::endl;
    int64_t mem_required = static_cast<int64_t>(n * sizeof(Node));
    double overhead = (mem_used - mem_required) * static_cast<double>(100) / mem_used;
    std::cout << "Overhead: " << std::fixed << std::setw(4) << std::setprecision(1) << overhead << '%' << std::endl;
}

int main(const int argc, const char** argv) {
    constexpr static unsigned N = 10000000;
    assignment_06::get_pool<assignment_06::pool>(N * sizeof(Node), sizeof(Node));
    std::cout << "Standard allocator:" << std::endl;
    test<assignment_06::standard_allocator_t>(N);
    std::cout << "Custom pool:" << std::endl;
    test<assignment_06::custom_pool_t>(N);
    return EXIT_SUCCESS;
}
