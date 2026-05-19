#include "sigsegv_handler.h"

#include <cstdlib>
#include <cstdio>
#include <signal.h>
#include <string_view>
#include <unistd.h>

namespace assignment_06 {

    static struct sigaction prev_handler;

    static void sigsegv_handler(int sig, siginfo_t* info, void* ucontext) {
        constexpr static std::string_view message("SIGSEGV for custom pool\n");
        write(STDERR_FILENO, message.data(), message.length());
        std::_Exit(EXIT_FAILURE);
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
