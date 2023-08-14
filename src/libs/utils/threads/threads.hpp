#pragma once
/**
 * @file threads.hpp
 * @brief Util for starting and managing threads within the system
 * @version 0.1
 *
 * @copyright Copyright (c) 2023
 *
 */

#include <iostream>
#include <atomic>
#include <thread>
#include <unistd.h>
#include <sys/syscall.h>

namespace utils::threads
{

    inline bool setThreadCore(int core_id) noexcept;

    /**
     * @brief Create and start running a new named thread, with affinity set to
     * specified core, and with a given function and arguments.
     *
     * @tparam Function the type of the function to be called
     * @tparam Args the types of the arguments to be passed to the function
     *
     * @param core_id Affinity of the thread will be set to this core
     * @param name Name given to the thread
     * @param func Function the thread will run
     * @param args Arguments to pass to the function (perfectly forwarded)
     *
     * @return new thread
     */
    template <typename Function, typename... Args>
    inline std::thread createAndStart(int core_id, const std::string& name, Function&& func, Args&&... args) noexcept {
        std::atomic<bool> running{false}, failed{false};
        auto args_tuple{std::make_tuple(std::forward<Args>(args)...)};

        // Body for every thread. This sets affinity, then runs the provided function, passing arguments.
        auto thread_body = [&running, &failed, &core_id, &name, &func, &args_tuple]() {
            if (core_id >= 0 && !setThreadCore(core_id)) [[unlikely]]
            {
                std::cerr << "Failed to set affinity for " << name << std::endl;
                failed = true;
                return;
            }
            std::cout << "Set core affinity for " << name << ": " << core_id << std::endl;
            running = true;
            std::apply(std::forward<Function>(func), args_tuple);
        };

        auto thread{std::thread{thread_body}};

        // Wait for thread to start running TODO switch to condition variable
        while (!running && !failed) {
            std::this_thread::sleep_for(std::chrono::seconds(1));
        }

        if (failed) [[unlikely]] {
            thread.join();
        }

        return thread;
    }

    /**
     * @brief Set the affinity of the calling thread to the specified core
     *
     * @param core_id Core ID to set affinity to
     * @returns true if successful, false otherwise
     */
    inline bool setThreadCore(int core_id) noexcept {
        cpu_set_t cpuset;
        CPU_ZERO(&cpuset);
        CPU_SET(core_id, &cpuset);

        // pthread API returns 0 on success
        int res{pthread_setaffinity_np(pthread_self(), sizeof(cpu_set_t), &cpuset)};
        return res == 0;
    }
}