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

#include <flaf.hpp>

#include <process.hpp>
#include <config.hpp>
#include <globals.hpp>
#include <ui.hpp>
#include <embed.hpp>

namespace cli {
int profile() {
    g->flag_root_profile() = flaf::next_argument();

    return 0;
}

namespace build {
std::vector<std::string> reconfigure_command_line() {
    return {
        g->cmake().path,
        std::format("-DBALK_UNCHANGABLE_SETTINGS_PROJECT={}", g->project()),
        "-S", g->root_path(),
        "-B", g->build_path(g->profile_name()),
        std::format("-DBALK_PROFILE={}", g->profile_name()),
        std::format("-DCMAKE_BUILD_TYPE={}", g->profile().cmake_build_type),
        std::format("-DBALK_SETTINGS_BINARY={}", g->binary()),
        "-G", g->build_system().build_type,
        std::format("-DCMAKE_MAKE_PROGRAM={}", g->build_system_name()),
        std::format("-DBALK_TOOLCHAIN={}", g->toolchain_name()),
        std::format("-DCMAKE_C_COMPILER={}", g->toolchain().c_compiler),
        std::format("-DCMAKE_CXX_COMPILER={}", g->toolchain().cpp_compiler),
        std::format("-DBALK_DIRECTORIES_BALK={}", g->balk_path()),
        std::format("-DBALK_DIRECTORIES_BUILD={}", g->build_path()),
        std::format("-DBALK_DIRECTORIES_BUILD_BENCHMARKS={}", g->benchmarks_build_path()),
        std::format("-DBALK_DIRECTORIES_BUILD_EXAMPLES={}", g->examples_build_path()),
        std::format("-DBALK_DIRECTORIES_BUILD_TESTS={}", g->tests_build_path()),
        std::format("-DBALK_DIRECTORIES_CMAKE={}", g->cmake_path()),
        std::format("-DBALK_DIRECTORIES_EXAMPLES={}", g->examples_path()),
        std::format("-DBALK_DIRECTORIES_EXPORT={}", g->export_path()),
        std::format("-DBALK_DIRECTORIES_HEADERS={}", g->headers_path()),
        std::format("-DBALK_DIRECTORIES_SOURCE={}", g->source_path()),
        std::format("-DBALK_DIRECTORIES_TESTS={}", g->tests_path()),
        std::format("-DBALK_DIRECTORIES_BENCHMARKS={}", g->benchmarks_path())
    };
}

std::vector<std::string> build_command_line() {
    return {g->cmake().path, "--build", g->build_path(g->profile_name())};
}

int main(std::vector<std::string> arguments) {
    //process::run(arguments);

    std::vector<std::string> command_line = {};

    command_line = reconfigure_command_line();
    std::cout << helper::dump_command_line("Reconfiguring", command_line) << "\n";
    process::run(command_line, true, true);

    command_line = build_command_line();
    std::cout << helper::dump_command_line("Building", command_line) << "\n";
    process::run(command_line, true, true);

    return 0;
}

struct Command {
    const static inline std::string short_help = "Build";
    const static inline std::string header_help = "Build";
    const static inline std::string footer_help = "Build";

    Command() {
        // create command accessed with say, requiring >= 0 arguments, calling say.
        flaf::command("build", flaf::C_or_cpp_function(main), 0, short_help, header_help, footer_help);

        // register enable_newlines to be called when -f flag passed to say command.
        //flaf::command_flags("say", "-n", enable_newlines, 0, "use_newlines");
    }
};

std::optional<Command> command = std::nullopt;
}

namespace run {
std::vector<std::string> run_command_line() {
    std::filesystem::path binary_path = g->build_path(g->profile_name());
    binary_path /= g->project();

    return {binary_path};
}

int main(std::vector<std::string> arguments) {
    //process::run(arguments);

    std::vector<std::string> command_line = run_command_line();
    for(const std::string &argument : arguments) {
        command_line.push_back(argument);
    }
    std::cout << helper::dump_command_line("Running", command_line) << "\n";
    process::run(command_line, true, true);

    return 0;
}

struct Command {
    const static inline std::string short_help = "Build";
    const static inline std::string header_help = "Build";
    const static inline std::string footer_help = "Build";

    Command() {
        // create command accessed with say, requiring >= 0 arguments, calling say.
        flaf::command("run", flaf::C_or_cpp_function(main), 0, short_help, header_help, footer_help);

        // register enable_newlines to be called when -f flag passed to say command.
        //flaf::command_flags("say", "-n", enable_newlines, 0, "use_newlines");
    }
};

std::optional<Command> command = std::nullopt;
}

namespace benchmarks {
std::vector<std::string> build_benchmarks_command_line() {
    return {g->cmake().path, "--build", g->build_path(g->profile_name()), "--target", "benchmarks"};
}

int main(std::vector<std::string> arguments) {
    std::vector<std::string> command_line = build_benchmarks_command_line();
    std::cout << helper::dump_command_line("Building", command_line) << "\n";
    process::run(command_line, true, true);

    std::filesystem::path root_path = g->build_path(g->profile_name());
    root_path /= g->benchmarks_build_path();

    std::vector<std::string> executables = {};
    std::error_code ec{};
    for (
        auto it = std::filesystem::recursive_directory_iterator(root_path, std::filesystem::directory_options::skip_permission_denied, ec);
        it != std::filesystem::recursive_directory_iterator();
        it.increment(ec))
    {
        if (ec) {
            ec.clear();
            continue;
        }

        if (it->is_regular_file()) {
            auto perms = it->status().permissions();

            // Check if owner, group, or others have execute permissions
            const auto executable_mask = (std::filesystem::perms::owner_exec | std::filesystem::perms::group_exec | std::filesystem::perms::others_exec);
            bool is_executable = ((perms & executable_mask) != std::filesystem::perms::none);

            if (is_executable) {
                executables.push_back(it->path());
            }
        }
    }

    command_line = std::vector<std::string>(1);
    for(const auto &executable : executables) {
        command_line[0] = executable;
        std::cout << helper::dump_command_line("Running", command_line) << "\n";
        process::run(command_line, true, true);
    }

    return 0;
}

struct Command {
    const static inline std::string short_help = "Benchmarks";
    const static inline std::string header_help = "Benchmarks";
    const static inline std::string footer_help = "Benchmarks";

    Command() {
        // create command accessed with say, requiring >= 0 arguments, calling say.
        flaf::command("benchmarks", flaf::C_or_cpp_function(main), 0, short_help, header_help, footer_help);

        // register enable_newlines to be called when -f flag passed to say command.
        //flaf::command_flags("say", "-n", enable_newlines, 0, "use_newlines");
    }
};

std::optional<Command> command = std::nullopt;
}

namespace benchmark {
int main(std::vector<std::string> arguments) {
    std::vector<std::string> command_line = ::cli::benchmarks::build_benchmarks_command_line();
    std::cout << helper::dump_command_line("Building", command_line) << "\n";
    process::run(command_line, true, true);

    std::filesystem::path root_path = g->build_path(g->profile_name());
    root_path /= g->benchmarks_build_path();

    bool can_run = false;
    command_line = std::move(arguments);

    const std::vector<std::string> suffixes = {"", ".exe"};
    for(const auto &suffix : suffixes) {
        const std::filesystem::path binary_path = root_path / (command_line[0] + suffix);
        const bool not_a_file(!std::filesystem::exists(binary_path) || !std::filesystem::is_regular_file(binary_path));
        if(not_a_file) {
            continue;
        }

        const auto perms = std::filesystem::status(binary_path).permissions();
        const auto executable_mask = (std::filesystem::perms::owner_exec | std::filesystem::perms::group_exec | std::filesystem::perms::others_exec);
        const bool not_executable((executable_mask & perms) == std::filesystem::perms::none);
        if(not_executable) {
            continue;
        }

        command_line[0] = binary_path;
        can_run = true;
        break;
    }
    assert(can_run);

    std::cout << helper::dump_command_line("Running benchmark", command_line) << "\n";
    process::run(command_line, true, true);

    return 0;
}

struct Command {
    const static inline std::string short_help = "Benchmark";
    const static inline std::string header_help = "Benchmark";
    const static inline std::string footer_help = "Benchmark";

    Command() {
        // create command accessed with say, requiring >= 0 arguments, calling say.
        flaf::command("benchmark", flaf::C_or_cpp_function(main), 1, short_help, header_help, footer_help);

        // register enable_newlines to be called when -f flag passed to say command.
        //flaf::command_flags("say", "-n", enable_newlines, 0, "use_newlines");
    }
};

std::optional<Command> command = std::nullopt;
}

namespace examples {
std::vector<std::string> build_examples_command_line() {
    return {g->cmake().path, "--build", g->build_path(g->profile_name()), "--target", "examples"};
}

int main(std::vector<std::string> arguments) {
    std::vector<std::string> command_line = build_examples_command_line();
    std::cout << helper::dump_command_line("Building", command_line) << "\n";
    process::run(command_line, true, true);

    std::filesystem::path root_path = g->build_path(g->profile_name());
    root_path /= g->examples_build_path();

    std::vector<std::string> executables = {};
    std::error_code ec{};
    for (
        auto it = std::filesystem::recursive_directory_iterator(root_path, std::filesystem::directory_options::skip_permission_denied, ec);
        it != std::filesystem::recursive_directory_iterator();
        it.increment(ec))
    {
        if (ec) {
            ec.clear();
            continue;
        }

        if (it->is_regular_file()) {
            auto perms = it->status().permissions();

            // Check if owner, group, or others have execute permissions
            const auto executable_mask = (std::filesystem::perms::owner_exec | std::filesystem::perms::group_exec | std::filesystem::perms::others_exec);
            bool is_executable = ((perms & executable_mask) != std::filesystem::perms::none);

            if (is_executable) {
                executables.push_back(it->path());
            }
        }
    }

    command_line = std::vector<std::string>(1);
    for(const auto &executable : executables) {
        command_line[0] = executable;
        std::cout << helper::dump_command_line("Running", command_line) << "\n";
        process::run(command_line, true, true);
    }

    return 0;
}

struct Command {
    const static inline std::string short_help = "Examples";
    const static inline std::string header_help = "Examples";
    const static inline std::string footer_help = "Examples";

    Command() {
        // create command accessed with say, requiring >= 0 arguments, calling say.
        flaf::command("examples", flaf::C_or_cpp_function(main), 0, short_help, header_help, footer_help);

        // register enable_newlines to be called when -f flag passed to say command.
        //flaf::command_flags("say", "-n", enable_newlines, 0, "use_newlines");
    }
};

std::optional<Command> command = std::nullopt;
}

namespace example {
int main(std::vector<std::string> arguments) {
    std::vector<std::string> command_line = ::cli::examples::build_examples_command_line();
    std::cout << helper::dump_command_line("Building", command_line) << "\n";
    process::run(command_line, true, true);

    std::filesystem::path root_path = g->build_path(g->profile_name());
    root_path /= g->examples_build_path();

    bool can_run = false;
    command_line = std::move(arguments);

    const std::vector<std::string> suffixes = {"", ".exe"};
    for(const auto &suffix : suffixes) {
        const std::filesystem::path binary_path = root_path / (command_line[0] + suffix);
        const bool not_a_file(!std::filesystem::exists(binary_path) || !std::filesystem::is_regular_file(binary_path));
        if(not_a_file) {
            continue;
        }

        const auto perms = std::filesystem::status(binary_path).permissions();
        const auto executable_mask = (std::filesystem::perms::owner_exec | std::filesystem::perms::group_exec | std::filesystem::perms::others_exec);
        const bool not_executable((executable_mask & perms) == std::filesystem::perms::none);
        if(not_executable) {
            continue;
        }

        command_line[0] = binary_path;
        can_run = true;
        break;
    }
    assert(can_run);

    std::cout << helper::dump_command_line("Running example", command_line) << "\n";
    process::run(command_line, true, true);

    return 0;
}

struct Command {
    const static inline std::string short_help = "Example";
    const static inline std::string header_help = "Example";
    const static inline std::string footer_help = "Example";

    Command() {
        // create command accessed with say, requiring >= 0 arguments, calling say.
        flaf::command("example", flaf::C_or_cpp_function(main), 1, short_help, header_help, footer_help);

        // register enable_newlines to be called when -f flag passed to say command.
        //flaf::command_flags("say", "-n", enable_newlines, 0, "use_newlines");
    }
};

std::optional<Command> command = std::nullopt;
}

namespace tests {
std::vector<std::string> build_tests_command_line() {
    return {g->cmake().path, "--build", g->build_path(g->profile_name()), "--target", "tests"};
}

int main(std::vector<std::string> arguments) {
    std::vector<std::string> command_line = build_tests_command_line();
    std::cout << helper::dump_command_line("Building", command_line) << "\n";
    process::run(command_line, true, true);

    std::filesystem::path root_path = g->build_path(g->profile_name());
    root_path /= g->tests_build_path();

    std::vector<std::string> executables = {};
    std::error_code ec{};
    for (
        auto it = std::filesystem::recursive_directory_iterator(root_path, std::filesystem::directory_options::skip_permission_denied, ec);
        it != std::filesystem::recursive_directory_iterator();
        it.increment(ec))
    {
        if (ec) {
            ec.clear();
            continue;
        }

        if (it->is_regular_file()) {
            auto perms = it->status().permissions();

            // Check if owner, group, or others have execute permissions
            const auto executable_mask = (std::filesystem::perms::owner_exec | std::filesystem::perms::group_exec | std::filesystem::perms::others_exec);
            bool is_executable = ((perms & executable_mask) != std::filesystem::perms::none);

            if (is_executable) {
                executables.push_back(it->path());
            }
        }
    }

    command_line = std::vector<std::string>(1);
    for(const auto &executable : executables) {
        command_line[0] = executable;
        std::cout << helper::dump_command_line("Running", command_line) << "\n";
        process::run(command_line, true, true);
    }

    return 0;
}

struct Command {
    const static inline std::string short_help = "Tests";
    const static inline std::string header_help = "Tests";
    const static inline std::string footer_help = "Tests";

    Command() {
        // create command accessed with say, requiring >= 0 arguments, calling say.
        flaf::command("tests", flaf::C_or_cpp_function(main), 0, short_help, header_help, footer_help);

        // register enable_newlines to be called when -f flag passed to say command.
        //flaf::command_flags("say", "-n", enable_newlines, 0, "use_newlines");
    }
};

std::optional<Command> command = std::nullopt;
}

namespace test {
int main(std::vector<std::string> arguments) {
    std::vector<std::string> command_line = ::cli::tests::build_tests_command_line();
    std::cout << helper::dump_command_line("Building", command_line) << "\n";
    process::run(command_line, true, true);

    std::filesystem::path root_path = g->build_path(g->profile_name());
    root_path /= g->tests_build_path();

    bool can_run = false;
    command_line = std::move(arguments);

    const std::vector<std::string> suffixes = {"", ".exe"};
    for(const auto &suffix : suffixes) {
        const std::filesystem::path binary_path = root_path / (command_line[0] + suffix);
        const bool not_a_file(!std::filesystem::exists(binary_path) || !std::filesystem::is_regular_file(binary_path));
        if(not_a_file) {
            continue;
        }

        const auto perms = std::filesystem::status(binary_path).permissions();
        const auto executable_mask = (std::filesystem::perms::owner_exec | std::filesystem::perms::group_exec | std::filesystem::perms::others_exec);
        const bool not_executable((executable_mask & perms) == std::filesystem::perms::none);
        if(not_executable) {
            continue;
        }

        command_line[0] = binary_path;
        can_run = true;
        break;
    }
    assert(can_run);

    std::cout << helper::dump_command_line("Running test", command_line) << "\n";
    process::run(command_line, true, true);

    return 0;
}

struct Command {
    const static inline std::string short_help = "Test";
    const static inline std::string header_help = "Test";
    const static inline std::string footer_help = "Test";

    Command() {
        // create command accessed with say, requiring >= 0 arguments, calling say.
        flaf::command("test", flaf::C_or_cpp_function(main), 1, short_help, header_help, footer_help);

        // register enable_newlines to be called when -f flag passed to say command.
        //flaf::command_flags("say", "-n", enable_newlines, 0, "use_newlines");
    }
};

std::optional<Command> command = std::nullopt;
}

namespace clean {
std::vector<std::string> clean_command_line() {
    return {g->cmake().path, "--build", g->build_path(g->profile_name()), "--target", "clean"};
}

int main(std::vector<std::string> arguments) {
    std::vector<std::string> command_line = {};

    command_line = clean_command_line();
    std::cout << helper::dump_command_line("Cleaning", command_line) << "\n";
    process::run(command_line, true, true);

    return 0;
}

struct Command {
    const static inline std::string short_help = "Clean";
    const static inline std::string header_help = "Clean";
    const static inline std::string footer_help = "Clean";

    Command() {
        // create command accessed with say, requiring >= 0 arguments, calling say.
        flaf::command("clean", flaf::C_or_cpp_function(main), 0, short_help, header_help, footer_help);

        // register enable_newlines to be called when -f flag passed to say command.
        //flaf::command_flags("say", "-n", enable_newlines, 0, "use_newlines");
    }
};

std::optional<Command> command = std::nullopt;
}

namespace project {
namespace create {
const std::string short_help("Create a project");
const std::string header_help("Create a project");
const std::string footer_help("Create a project");

int main(std::vector<std::string> arguments) {
    const std::string project_name = arguments[0];
    const std::filesystem::path project_name_path = arguments[0];
    const std::filesystem::path balk_path(project_name_path / "balk");
    const std::filesystem::path benchmarks_path(project_name_path / "benchmarks");
    const std::filesystem::path build_path(project_name_path / "build");
    const std::filesystem::path cmake_path(project_name_path / "cmake");
    const std::filesystem::path cmake_fetch_path(cmake_path / "fetch");
    const std::filesystem::path examples_path(project_name_path / "examples");
    const std::filesystem::path export_path(project_name_path / "export");
    const std::filesystem::path headers_path(project_name_path / "headers");
    const std::filesystem::path source_path(project_name_path / "source");
    const std::filesystem::path tests_path(project_name_path / "tests");

    std::filesystem::create_directories(project_name_path);
    std::filesystem::create_directories(balk_path);
    std::filesystem::create_directories(benchmarks_path);
    std::filesystem::create_directories(build_path);
    std::filesystem::create_directories(cmake_path);
    std::filesystem::create_directories(cmake_fetch_path);
    std::filesystem::create_directories(examples_path);
    std::filesystem::create_directories(export_path);
    std::filesystem::create_directories(headers_path);
    std::filesystem::create_directories(source_path);
    std::filesystem::create_directories(tests_path);

    {
        const std::filesystem::path file_path(project_name_path / "balk.json");

        json created_config_json = json::parse(embed::balk_json);
        config::Config created_config = created_config_json;
        created_config.unchangable_settings.project = project_name;
        created_config_json = created_config;
        const std::string created_config_json_string = created_config_json.dump(4);

        helper::dump_all(file_path, created_config_json_string);
    }

    helper::dump_all(project_name_path / "CMakeLists.txt", embed::cmakelists_txt);
    helper::dump_all(source_path / "main.cpp", embed::sources::main_cpp);
    helper::dump_all(benchmarks_path / "benchmark.cpp", embed::benchmarks::benchmark_cpp);
    helper::dump_all(examples_path / "example.cpp", embed::examples::example_cpp);
    helper::dump_all(tests_path / "test.cpp", embed::tests::test_cpp);

    helper::dump_all(cmake_path / "build_benchmarks.cmake", embed::cmake::build_benchmarks_cmake);
    helper::dump_all(cmake_path / "build_binary.cmake", embed::cmake::build_binary_cmake);
    helper::dump_all(cmake_path / "build_examples.cmake", embed::cmake::build_examples_cmake);
    helper::dump_all(cmake_path / "build_tests.cmake", embed::cmake::build_tests_cmake);
    helper::dump_all(cmake_path / "command_targets.cmake", embed::cmake::command_targets_cmake);
    helper::dump_all(cmake_path / "compilation.cmake", embed::cmake::compilation_cmake);
    helper::dump_all(cmake_path / "fetch.cmake", embed::cmake::fetch_cmake);
    helper::dump_all(cmake_path / "headers.cmake", embed::cmake::headers_cmake);
    helper::dump_all(cmake_path / "modules.cmake", embed::cmake::modules_cmake);
    helper::dump_all(cmake_path / "module_test_capability.cmake", embed::cmake::module_test_capability_cmake);

    return EXIT_SUCCESS;
}
}

namespace adopt {
const std::string short_help("Adopt a project");
const std::string header_help("Adopt a project");
const std::string footer_help("Adopt a project");

int main(std::vector<std::string> arguments) {
    std::vector<std::string> command_line = {};

    g->signals.check_brutal();
    std::filesystem::create_directory("build");
    command_line = {"cmake", "-S", ".", "-B", "build/default"};
    const std::optional<bool> maybe_use_custom_reconfigure_flags = ui::yes_no_prompt("Use custom reconfigure flags?");
    g->signals.check_brutal();
    if(maybe_use_custom_reconfigure_flags && (*maybe_use_custom_reconfigure_flags)) {
        const std::optional<std::string> maybe_flat_reconfigure_flags = ui::respond("Custom reconfiguration flags:");
        if(maybe_flat_reconfigure_flags) {
            const std::string flat_reconfigure_flags = *maybe_flat_reconfigure_flags;
            command_line.push_back(flat_reconfigure_flags);
        }
    }
    g->signals.check_brutal();
    process::run(command_line, true, true);
    g->signals.check_brutal();

    g->signals.check_brutal();
    command_line = {"cmake", "--build", "build/default"};
    const std::optional<bool> maybe_use_custom_build_flags = ui::yes_no_prompt("Use custom build flags?");
    g->signals.check_brutal();
    if(maybe_use_custom_build_flags && (*maybe_use_custom_build_flags)) {
        const std::optional<std::string> maybe_flat_build_flags = ui::respond("Custom build flags:");
        if(maybe_flat_build_flags) {
            const std::string flat_build_flags = *maybe_flat_build_flags;
            command_line.push_back(flat_build_flags);
        }
    }
    g->signals.check_brutal();
    process::run(command_line, true, true);
    g->signals.check_brutal();

    return EXIT_SUCCESS;
}
}

namespace destroy {
const std::string short_help("Destory a project");
const std::string header_help("Destroy a project");
const std::string footer_help("Destroy a project");

int main(std::vector<std::string> arguments) {
    g->signals.check_brutal();
    const std::optional<bool> maybe_perform_deletion = ui::yes_no_prompt("Really delete the project?");
    g->signals.check_brutal();

    if(maybe_perform_deletion && (*maybe_perform_deletion)) {
        std::filesystem::path balk_json_path = g->root_path();
        balk_json_path /= "balk.json";
        std::filesystem::remove(balk_json_path);
    }

    return EXIT_SUCCESS;
}
}

namespace makefile {
const std::string short_help("Dump workflows to a Makefile");
const std::string header_help("Dump workflows to a Makefile");
const std::string footer_help("Dump workflows to a Makefile");

int main(std::vector<std::string> arguments) {
    std::unordered_map<std::string, std::vector<std::vector<std::string>>> command_invocations = {};

    std::filesystem::path makefile_path = g->root_path();
    makefile_path /= "Makefile";
    // open makefile

    command_invocations["build"] = {};
    command_invocations["build"].push_back(::cli::build::reconfigure_command_line());
    command_invocations["build"].push_back(::cli::build::build_command_line());

    command_invocations["run"] = {};
    command_invocations["run"].push_back(::cli::run::run_command_line());

    command_invocations["clean"] = {};
    command_invocations["clean"].push_back(::cli::clean::clean_command_line());

    for(const auto& [command, invocations] : command_invocations) {
        std::cout << std::format("{}:\n", command);
        for(const std::vector<std::string> &invocation : invocations) {
            std::cout << std::format("\t{}\n", helper::dump_command_line(std::nullopt, invocation));
        }
    }

    return EXIT_SUCCESS;
}
}

struct Subroot  {
    const static inline std::string short_help = "Manage a project";
    const static inline std::string header_help = "Manage a project";
    const static inline std::string footer_help = "Manage a project";

    Subroot() {
        flaf::subroot("project", short_help, header_help, footer_help);
        //flaf::subroot_flags("work", "-e", e, 0, "this is e!");
        //flaf::subroot_flags("work", "-f", f, 0, "this is f!");

        flaf::subcommand("project", "create", flaf::C_or_cpp_function(create::main), 3, create::short_help, create::header_help, create::footer_help);

        flaf::subcommand("project", "adopt", flaf::C_or_cpp_function(adopt::main), 0, adopt::short_help, adopt::header_help, adopt::footer_help);

        flaf::subcommand("project", "destroy", flaf::C_or_cpp_function(destroy::main), 0, destroy::short_help, destroy::header_help, destroy::footer_help);

        flaf::subcommand("project", "makefile", flaf::C_or_cpp_function(makefile::main), 0, makefile::short_help, makefile::header_help, makefile::footer_help);
    }
};

std::optional<Subroot> subroot = std::nullopt;
}

int main(int argc, char *argv[]) {
    std::string program_name = "balk";
    std::string program_header = "A workflow tool for CMake.\n";
    std::string program_footer = "Not finished.\n";

    // set command name and help header/footer.
    flaf::root_set(program_name, program_header, program_footer);
    flaf::root_flags("-p", profile, 1, "Select a profile");
    flaf::root_flags("--profile", profile, 1, "Select a profile");

    benchmark::command = benchmark::Command();
    benchmarks::command = benchmarks::Command();
    build::command = build::Command();
    clean::command = clean::Command();
    example::command = example::Command();
    examples::command = examples::Command();
    run::command = run::Command();
    test::command = test::Command();
    tests::command = tests::Command();

    project::subroot = project::Subroot();

    // handle user arguments using defined commands/subcommands
    return flaf::main(argc, argv);
}
}
