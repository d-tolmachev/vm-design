#include "safe_read_uint8.h"

#include <array>
#include <cstdlib>
#include <cstdio>
#include <setjmp.h>
#include <signal.h>
#include <string_view>
#include <unistd.h>

namespace assignment_09 {

    static std::optional<const uint8_t*> saved_ptr = std::nullopt;

    static struct sigaction prev_sigbus_handler;
    static struct sigaction prev_sigsegv_handler;
    static sigjmp_buf env;

    static void sig_handler(int sig, siginfo_t* info, void* ucontext) {
        const uint8_t* mem_hit = static_cast<const uint8_t*>(info->si_addr);
        if (saved_ptr && *saved_ptr == mem_hit) {
            static std::array<std::string_view, 3> messages{{"Custom ", sig == SIGBUS ? "sigbus" : "sigsegv", " handler\n"}};
            for (std::string_view message : messages) {
                write(STDOUT_FILENO, message.data(), message.length());
            }
            siglongjmp(env, 1);
        }
        struct sigaction sa = sig == SIGBUS ? prev_sigbus_handler : prev_sigsegv_handler;
        if (sa.sa_handler == SIG_DFL) {
            sigaction(sig, &sa, nullptr);
            raise(sig);
        } else if (sa.sa_handler != SIG_IGN) {
            sa.sa_sigaction(sig, info, ucontext);
        }
    }

    static bool is_canonical(uintptr_t addr) {
        return ((static_cast<uint64_t>(addr) << 16) >> 16) == static_cast<uint64_t>(addr);
    }

    std::optional<uint8_t> safe_read_uint8(const uint8_t* ptr) {
        if (!is_canonical(reinterpret_cast<uintptr_t>(ptr))) {
            return std::nullopt;
        }
        struct sigaction sa;
        sa.sa_flags = SA_SIGINFO;
        sa.sa_sigaction = sig_handler;
        sigemptyset(&sa.sa_mask);
        if (sigaction(SIGBUS, &sa, &prev_sigbus_handler) != 0 || sigaction(SIGSEGV, &sa, &prev_sigsegv_handler) != 0) {
            return std::nullopt;
        }
        saved_ptr = ptr;
        std::optional<uint8_t> result;
        if (sigsetjmp(env, 1) == 0) {
            result = *ptr;
        } else {
            result = std::nullopt;
        }
        if (sigaction(SIGBUS, &prev_sigbus_handler, nullptr) != 0 || sigaction(SIGSEGV, &prev_sigsegv_handler, nullptr) != 0) {
            return std::nullopt;
        }
        return result;
    }

}
