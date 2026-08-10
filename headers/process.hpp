#pragma once

#include <future>
#include <iostream>
#include <mutex>
#include <unordered_map>
#include <string>
#include <filesystem>
#include <fstream>
#include <functional>
#include <memory>
#include <csignal>
#include <cstdlib>
#include <optional>

#include <reproc++/drain.hpp>
#include <reproc++/reproc.hpp>

namespace process {
inline std::optional<std::error_code> run(const std::vector<std::string> &arguments, bool show_stdout, bool show_stderr) {
    reproc::process process{};
    reproc::options options{};

    options.redirect.in = reproc::redirect{reproc::redirect::pipe};
    options.redirect.out = reproc::redirect{reproc::redirect::pipe};
    options.redirect.err = reproc::redirect{reproc::redirect::pipe};

    std::error_code ec = process.start(arguments, options);
    if(ec) {
        if (ec == std::errc::no_such_file_or_directory) {
            std::cerr << "Program not found. Make sure it's available from the PATH.";
        }
        return ec;
    }

    // We need a mutex along with `output` to prevent the main thread and
    // background thread from modifying `output` at the same time (`std::string`
    // is not thread safe).
    std::string stdout_output;
    std::string stderr_output;
    std::mutex mutex;

    auto drain_async = std::async(std::launch::async, [&]() {
        // `sink::thread_safe::string` locks a given mutex before appending to the
        // given string, allowing working with the string across multiple threads if
        // the mutex is locked in the other threads as well.
        reproc::sink::thread_safe::string stdout_sink(stdout_output, mutex);
        reproc::sink::thread_safe::string stderr_sink(stderr_output, mutex);
        return reproc::drain(process, stdout_sink, stderr_sink);
    });

    // Show new output every 2 seconds.
    while (drain_async.wait_for(std::chrono::milliseconds(250)) != std::future_status::ready) {
        std::lock_guard<std::mutex> lock(mutex);

        if(show_stdout && !(stdout_output.empty())) {
            std::cout << "cout:\n";
            std::cout << stdout_output;
            std::cout << std::endl;
        }


        if(show_stderr && !(stderr_output.empty())) {
            std::cout << "cerr:\n";
            std::cout << stderr_output;
            std::cout << std::endl;
        }

        // Clear output that's already been flushed to `std::cout`.
        stdout_output.clear();
        stderr_output.clear();
    }

    // Flush any remaining output of `process`.
    {
        std::lock_guard<std::mutex> lock(mutex);

        if(show_stdout && !(stdout_output.empty())) {
            std::cout << "cout:\n";
            std::cout << stdout_output;
            std::cout << std::endl;
        }


        if(show_stderr && !(stderr_output.empty())) {
            std::cout << "cerr:\n";
            std::cout << stderr_output;
            std::cout << std::endl;
        }
    }

    // Check if any errors occurred in the background thread.
    ec = drain_async.get();
    if (ec) {
        return ec;
    }

    reproc::stop_actions stop = {
        { reproc::stop::terminate, reproc::milliseconds(5000) },
        { reproc::stop::kill, reproc::milliseconds(2000) },
        {}
    };

    int status = 0;
    std::tie(status, ec) = process.stop(stop);
    if (ec) {
        return ec;
    }

    return std::nullopt;
}
}
