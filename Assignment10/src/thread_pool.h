#ifndef THREAD_POOL_H
#define THREAD_POOL_H

#include <concepts>
#include <condition_variable>
#include <iostream>
#include <mutex>
#include <queue>
#include <stdexcept>
#include <utility>
#include <thread>
#include <vector>

namespace assignment_10 {

    template <std::invocable T>
    class thread_pool {
    public:
        explicit thread_pool(size_t threads_cnt = std::thread::hardware_concurrency());

        thread_pool(const thread_pool& other) = delete;

        thread_pool(thread_pool&& other) noexcept = delete;

        thread_pool& operator=(const thread_pool& rhs) = delete;

        thread_pool& operator=(thread_pool&& other) noexcept = delete;

        ~thread_pool();

        [[nodiscard]] bool is_stop() const noexcept;

        void stop() noexcept;

        template <class... Args>
        void emplace(Args&&... args);

    private:
        std::queue<T> actions_;
        mutable std::mutex mutex_;
        mutable std::condition_variable cond_;
        bool stop_;
        std::vector<std::thread> workers_;

        void execute_actions();
    };

    template <std::invocable T>
    thread_pool<T>::thread_pool(size_t threads_cnt)
        : stop_(false) {
        workers_.reserve(threads_cnt);
        while (threads_cnt > 0) {
            auto execute = [this]() {
                execute_actions();
            };
            workers_.emplace_back(execute);
            --threads_cnt;
        }
    }

    template <std::invocable T>
    thread_pool<T>::~thread_pool() {
        if (!stop_) {
            stop();
        }
    }

    template <std::invocable T>
    bool thread_pool<T>::is_stop() const noexcept {
        std::scoped_lock<std::mutex> lock(mutex_);
        return stop_;
    }

    template <std::invocable T>
    void thread_pool<T>::stop() noexcept {
        {
            std::scoped_lock<std::mutex> lock(mutex_);
            stop_ = true;
        }
        cond_.notify_all();
        for (std::thread& worker : workers_) {
            worker.join();
        }
        workers_.clear();
    }

    template <std::invocable T>
    template <class... Args>
    void thread_pool<T>::emplace(Args&&... args) {
        {
            std::scoped_lock<std::mutex> lock(mutex_);
            actions_.emplace(std::forward<Args>(args)...);
        }
        cond_.notify_one();
    }

    template <std::invocable T>
    void thread_pool<T>::execute_actions() {
        std::unique_lock<std::mutex> lock(mutex_);
        while (!stop_) {
            while (!stop_ && actions_.empty()) {
                cond_.wait(lock);
            }
            if (actions_.empty()) {
                return;
            }
            T action = std::move(actions_.front());
            actions_.pop();
            lock.unlock();
            try {
                action();
            } catch (const std::exception& e) {
                std::cerr << "Execution failed: " << e.what() << std::endl;
            }
            lock.lock();
        }
    }

}

#endif
