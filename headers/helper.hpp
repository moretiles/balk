#pragma once

#include <rapidfuzz/fuzz.hpp>

#include <algorithm>
#include <chrono>
#include <filesystem>
#include <optional>
#include <string>
#include <system_error>

namespace helper {
inline std::optional<std::string> current_or_parent_directory_containing_file(std::string filename) {
    std::filesystem::path current = std::filesystem::current_path();

    while (true) {
        std::filesystem::path candidate = current / filename;

        if (std::filesystem::exists(candidate)) {
            return current;
        }

        // Stop if we've reached the filesystem root.
        if (current == current.root_path()) {
            return std::nullopt;
        }

        current = current.parent_path();
    }

    return std::nullopt;
}

inline std::optional<std::int64_t> get_timestamp_for_directory_and_children(const std::filesystem::path& directory_path) {
    std::error_code ec{};

    // Get root directory time safely
    auto newest = std::filesystem::last_write_time(directory_path, ec);
    if (ec) {
        return std::nullopt;
    }

    // Configure iterator to skip permission errors automatically
    auto options = std::filesystem::directory_options::skip_permission_denied;
    auto it = std::filesystem::recursive_directory_iterator(directory_path, options, ec);
    if (ec) {
        return std::nullopt;
    }

    for (const auto& entry : it) {
        // Use entry.last_write_time(ec) directly instead of entry.path() to avoid symlink crashes
        auto current_time = entry.last_write_time(ec);

        // Skip files that throw errors (like broken symlinks or locked files)
        if (!ec) {
            newest = std::max(newest, current_time);
        }
    }

    const auto sys_time = std::chrono::clock_cast<std::chrono::system_clock>(newest);
    const auto duration_time = std::chrono::duration_cast<std::chrono::seconds>(sys_time.time_since_epoch());
    return duration_time.count();
}

void fuzzy_sort(std::vector<std::string>& items, const std::string& query) {
    std::sort(items.begin(), items.end(),
    [&](const std::string& a, const std::string& b) {
        return rapidfuzz::fuzz::WRatio(query, a) >
               rapidfuzz::fuzz::WRatio(query, b);
    });
}

std::string dump_command_line(const std::optional<std::string> maybe_explanation, const std::vector<std::string> &command_line) {
    std::string return_me = "";
    if(maybe_explanation) {
        return_me += (*maybe_explanation);
        return_me += ": ";
    }

    if(command_line.size() > 0) {
        return_me += command_line[0];
        for(std::uint64_t i = 1; i < command_line.size(); i++) {
            return_me += " ";
            return_me += command_line[i];
        }
    }

    return return_me;
}

std::string load_all(const std::filesystem::path& path) {
    std::ifstream file(path, std::ios::binary);

    if (!file) {
        const auto io_error = std::make_error_code(std::errc::io_error);
        throw std::system_error(io_error, "load_all: " + path.string());
    }

    file.seekg(0, std::ios::end);
    const auto size = file.tellg();

    if (size < 0) {
        const auto io_error = std::make_error_code(std::errc::io_error);
        throw std::system_error(io_error, "load_all: " + path.string());
    }

    std::string result(static_cast<std::size_t>(size), '\0');

    file.seekg(0, std::ios::beg);
    if (!(file.read(result.data(), result.size()))) {
        const auto io_error = std::make_error_code(std::errc::io_error);
        throw std::system_error(io_error, "load_all: " + path.string());
    }

    return result;
}

void dump_all(const std::filesystem::path& path, const std::string &data) {
    std::ofstream file(path, std::ios::binary | std::ios::trunc);

    if (!file) {
        const auto io_error = std::make_error_code(std::errc::io_error);
        throw std::system_error(io_error, "dump_all: " + path.string());
    }

    file.write(data.data(), static_cast<std::streamsize>(data.size()));
    if (!file) {
        const auto io_error = std::make_error_code(std::errc::io_error);
        throw std::system_error(io_error, "dump_all: " + path.string());
    }
}

void dump_all(const std::filesystem::path& path, const std::string_view &data) {
    std::ofstream file(path, std::ios::binary | std::ios::trunc);

    if (!file) {
        const auto io_error = std::make_error_code(std::errc::io_error);
        throw std::system_error(io_error, "dump_all: " + path.string());
    }

    file.write(data.data(), static_cast<std::streamsize>(data.size()));
    if (!file) {
        const auto io_error = std::make_error_code(std::errc::io_error);
        throw std::system_error(io_error, "dump_all: " + path.string());
    }
}
}
