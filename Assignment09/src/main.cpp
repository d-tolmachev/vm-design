#include <cassert>
#include <cstdint>
#include <cstdlib>
#include <cstdio>
#include <iostream>
#include <optional>
#include <setjmp.h>
#include <signal.h>
#include <string_view>
#include <unistd.h>

#include "safe_read_uint8.h"

template <class TestCase>
void test(const std::string& test_name, TestCase& test_case) {
    std::cout << test_name << " started" << std::endl;
    test_case();
    std::cout << test_name << " finished" << std::endl << std::endl << std::endl;
}

void test_basic() {
    int num = 42;
    assert(*assignment_09::safe_read_uint8(static_cast<const uint8_t*>(static_cast<const void*>(&num))) == 42);
    constexpr std::string_view message("some string");
    assert(*assignment_09::safe_read_uint8(static_cast<const uint8_t*>(static_cast<const void*>(message.data()))) == 's');
    for (int i = 0; i < num; ++i) {
        assert(*assignment_09::safe_read_uint8(static_cast<const uint8_t*>(static_cast<const void*>(&num))) == 42);
    }
    assert(assignment_09::safe_read_uint8(nullptr) == std::nullopt);
    assert(assignment_09::safe_read_uint8(reinterpret_cast<const uint8_t*>(num)) == std::nullopt);
}

void test_is_canonical() {
    assert(assignment_09::safe_read_uint8(reinterpret_cast<const uint8_t*>(0xffffffffffffffff)) == std::nullopt);
}

#ifndef SA_RESTORER
#define SA_RESTORER 0x04000000
#endif

void test_prev_handler() {
    struct sigaction prev_sigbus_handler;
    struct sigaction prev_sigsegv_handler;
    sigaction(SIGBUS, nullptr, &prev_sigbus_handler);
    sigaction(SIGSEGV, nullptr, &prev_sigsegv_handler);
    int num = 42;
    std::optional<uint8_t> result = *assignment_09::safe_read_uint8(static_cast<const uint8_t*>(static_cast<const void*>(&num)));
    assert(result && *result == 42);
    struct sigaction current_sigbus_handler;
    struct sigaction current_sigsegv_handler;
    sigaction(SIGBUS, nullptr, &current_sigbus_handler);
    sigaction(SIGSEGV, nullptr, &current_sigsegv_handler);
    assert(current_sigbus_handler.sa_sigaction == prev_sigbus_handler.sa_sigaction);
    assert((current_sigbus_handler.sa_flags & ~SA_RESTORER) == (prev_sigbus_handler.sa_flags & ~SA_RESTORER));
    assert(current_sigsegv_handler.sa_sigaction == prev_sigsegv_handler.sa_sigaction);
    assert((current_sigsegv_handler.sa_flags & ~SA_RESTORER) == (prev_sigsegv_handler.sa_flags & ~SA_RESTORER));
}

static bool sigsegv_handler_called = false;

static sigjmp_buf env;

static void sigsegv_handler(int sig, siginfo_t* info, void* ucontext) {
    sigsegv_handler_called = true;
    constexpr static std::string_view message("External signal handler\n");
    write(STDOUT_FILENO, message.data(), message.length());
    siglongjmp(env, 1);
}

void test_external_handler() {
    struct sigaction prev_sigsegv_handler;
    struct sigaction sa;
    sa.sa_flags = SA_SIGINFO;
    sa.sa_sigaction = sigsegv_handler;
    sigemptyset(&sa.sa_mask);
    assert(sigaction(SIGSEGV, &sa, &prev_sigsegv_handler) == 0);
    const uint8_t* ptr = reinterpret_cast<const uint8_t*>(42);
    sigsegv_handler_called = false;
    std::optional<uint8_t> first = std::nullopt;
    if (sigsetjmp(env, 1) == 0) {
        first = *ptr;
    } else {
        first = std::nullopt;
    }
    assert(sigsegv_handler_called);
    assert(first == std::nullopt);
    sigsegv_handler_called = false;
    std::optional<uint8_t> second = assignment_09::safe_read_uint8(ptr);
    assert(!sigsegv_handler_called);
    assert(second == std::nullopt);
    sigaction(SIGSEGV, &prev_sigsegv_handler, nullptr);
}

int main() {
    test("basic test", test_basic);
    test("is canonical test", test_is_canonical);
    test("prev handler test", test_prev_handler);
    test("external handler test", test_external_handler);
    return 0;
}
