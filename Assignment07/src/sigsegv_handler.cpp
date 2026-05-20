#include "sigsegv_handler.h"

#include <array>
#include <cstddef>
#include <cstdlib>
#include <cstdio>
#include <signal.h>
#include <string_view>
#include <unistd.h>

#include "pool_registry.h"

namespace assignment_07 {

    static struct sigaction prev_handler;

    static void sigsegv_handler(int sig, siginfo_t* info, void* ucontext) {
        std::byte* mem_hit = static_cast<std::byte*>(info->si_addr);
        size_t pool_id = pool_registry::get_or_create()->find_pool(mem_hit);
        if (pool_id != pool_registry::npos) {
            static std::array<char, 21> string_pool_id;
            size_t digit = pool_id != 0 ? 0 : 1;
            for (size_t i = pool_id; i > 0; i /= 10) {
                ++digit;
            }
            string_pool_id[digit] = '\0';
            while (digit > 0) {
                --digit;
                string_pool_id[digit] = static_cast<char>('0' + pool_id % 10);
                pool_id /= 10;
            }
            static std::array<std::string_view, 3> messages{"SIGSEGV for pool", string_pool_id.data(), "\n"};
            for (std::string_view message : messages) {
                write(STDERR_FILENO, message.data(), message.length());
            }
            std::_Exit(EXIT_FAILURE);
        }
        if (prev_handler.sa_handler == SIG_DFL) {
            sigaction(sig, &prev_handler, nullptr);
            raise(sig);
        } else if (prev_handler.sa_handler != SIG_IGN) {
            prev_handler.sa_sigaction(sig, info, ucontext);
        }
    }

    void register_sigsegv_handler() {
        struct sigaction sa;
        sa.sa_flags = SA_SIGINFO;
        sa.sa_sigaction = sigsegv_handler;
        sigemptyset(&sa.sa_mask);
        if (sigaction(SIGSEGV, &sa, &prev_handler) != 0) {
            std::perror("sigaction failed");
            std::exit(EXIT_FAILURE);
        }
    }

}
