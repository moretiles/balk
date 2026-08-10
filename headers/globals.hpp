#pragma once

#include <cassert>
#include <csignal>
#include <cstdlib>
#include <filesystem>
#include <fstream>
#include <functional>
#include <future>
#include <iostream>
#include <memory>
#include <mutex>
#include <optional>
#include <string>
#include <unordered_map>
#include <vector>

#include <nlohmann/json.hpp>
using json = nlohmann::json;

#include <nlohmann/json-schema.hpp>
using json_validator = nlohmann::json_schema::json_validator;

#include <helper.hpp>
#include <config.hpp>

namespace globals {
struct MetaInformation {
    std::string project_root_directory_path = "";

    MetaInformation() {
        auto maybe_project_root_directory_path = helper::current_or_parent_directory_containing_file("balk.json");
        assert(maybe_project_root_directory_path);
        project_root_directory_path = *maybe_project_root_directory_path;
    }
};

struct Flags {
    std::optional<std::string> profile = std::nullopt;
    std::int64_t verbosity = 0;

    Flags() {}
};

enum class Signame {
    INTERRUPT
};

struct Signals {
    std::atomic<bool> interrupted{false};

    static void handle_interrupt(int signal);

    Signals() {
        std::signal(SIGINT, handle_interrupt);
    }

    void clear() {
        interrupted = false;
    }

    std::unordered_map<Signame, bool> check() {
        std::unordered_map<Signame, bool> return_value = {};

        return_value[Signame::INTERRUPT] = interrupted;

        return return_value;
    }

    void check_brutal() {
        const std::unordered_map<Signame, bool> signal_map = check();
        bool any_signal_present = false;
        for(const auto [signal_name, signal_status] : signal_map) {
            if(signal_status) {
                any_signal_present = true;
            }
        }

        if(any_signal_present) {
            std::cerr << "Exiting due to one or more signals!" << std::endl;
            std::exit(1);
        }
    }
};

class Globals {
private:
    MetaInformation meta_information = MetaInformation();
    Flags flags = Flags();

public:
    config::Config config{};
    Signals signals = Signals();

public:
    Globals() {
        if(0) {
            config::Config unset_config{};
            json j = unset_config;
            //std::cout << j.dump() << "\n";
        } else {
            std::filesystem::path balk_json = meta_information.project_root_directory_path;
            balk_json /= "balk.json";
            std::ifstream file(balk_json);

            assert(file);

            json j;
            file >> j;

            json schema = json::parse(config::config_schema);
            json_validator validator{};
            validator.set_root_schema(schema);
            validator.validate(j);

            config = j.get<config::Config>();

            json output = config;
            //std::cout << output.dump(4) << '\n';
        }
    }

    const std::string &version() {
        return config.version;
    }

    const std::string &project() {
        return config.unchangable_settings.project;
    }

    const std::string &language() {
        return config.unchangable_settings.language;
    }

    const std::string &meta_build_system() {
        return config.unchangable_settings.meta_build_system;
    }

    const std::string &binary() {
        return config.settings.binary;
    }

    const config::Cmake &cmake() {
        return config.settings.cmake;
    }

    const std::string &profile_name() {
        if(flags.profile) {
            return *(flags.profile);
        }

        return config.profiles.default_name;
    }

    const std::unordered_map<std::string, config::ProfileEntry> &profiles() {
        return config.profiles.known;
    }

    const config::ProfileEntry &profile(std::optional<std::string> maybe_profile_name = std::nullopt) {
        if(!maybe_profile_name) {
            maybe_profile_name = profile_name();
        }

        return profiles().at(*maybe_profile_name);
    }

    const std::unordered_map<std::string, config::BuildSystem> &build_systems() {
        return config.build_systems;
    }

    const config::BuildSystem &build_system(std::optional<std::string> build_system_name = std::nullopt) {
        if(!build_system_name) {
            build_system_name = config.settings.build_system;
        }

        return build_systems().at(*build_system_name);
    }

    const std::unordered_map<std::string, config::Toolchain> &toolchains() {
        return config.toolchains;
    }

    const config::Toolchain &toolchain(std::optional<std::string> toolchain_name = std::nullopt) {
        if(!toolchain_name) {
            toolchain_name = config.settings.toolchain;
        }

        return toolchains().at(*toolchain_name);
    }

    const std::unordered_map<std::string, config::FormatProgram> &format_programs() {
        return config.format.known;
    }

    const config::FormatProgram &format_program(std::optional<std::string> format_program_name = std::nullopt) {
        if(!format_program_name) {
            format_program_name = config.format.selected;
        }

        return format_programs().at(*format_program_name);
    }

    const std::unordered_map<std::string, config::LintProgram> &lint_programs() {
        return config.lint.known;
    }

    const config::LintProgram &lint_program(std::optional<std::string> lint_program_name = std::nullopt) {
        if(!lint_program_name) {
            lint_program_name = config.lint.selected;
        }

        return lint_programs().at(*lint_program_name);
    }

    const std::unordered_map<std::string, config::Command> &commands() {
        return config.commands;
    }

    const config::Command &command(const std::string &command_name) {
        return commands().at(command_name);
    }

    const std::string &root_path() {
        return meta_information.project_root_directory_path;
    }

    const std::string balk_path() {
        std::filesystem::path path = root_path();
        path /= config.directories.balk_path;
        return path;
    }

    const std::string build_path() {
        std::filesystem::path path = root_path();
        path /= config.directories.build_path;
        return path;
    }

    const std::string build_path(const std::string &profile_name) {
        std::filesystem::path path = root_path();
        path /= config.directories.build_path;
        path /= profile_name;
        return path;
    }

    const std::string cmake_path() {
        std::filesystem::path path = root_path();
        path /= config.directories.cmake_path;
        return path;
    }

    const std::string examples_path() {
        std::filesystem::path path = root_path();
        path /= config.directories.examples_path;
        return path;
    }

    const std::string export_path() {
        std::filesystem::path path = root_path();
        path /= config.directories.export_path;
        return path;
    }

    const std::string headers_path() {
        std::filesystem::path path = root_path();
        path /= config.directories.headers_path;
        return path;
    }

    const std::string source_path() {
        std::filesystem::path path = root_path();
        path /= config.directories.source_path;
        return path;
    }

    const std::string tests_path() {
        std::filesystem::path path = root_path();
        path /= config.directories.tests_path;
        return path;
    }

    const std::string benchmarks_path() {
        std::filesystem::path path = root_path();
        path /= config.directories.benchmarks_path;
        return path;
    }

    std::optional<std::string> &flag_root_profile() {
        return flags.profile;
    }
};
}

extern globals::Globals *g;

// have to indirectly make use of g for the function argument to std::signal
void globals::Signals::handle_interrupt(int signal) {
    if(signal == SIGINT) {
        g->signals.interrupted = true;
    }
}
