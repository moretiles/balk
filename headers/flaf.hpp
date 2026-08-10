#pragma once

#include <string>
#include <variant>
#include <functional>
#include <vector>
#include <unordered_map>
#include <cassert>
#include <iostream>
#include <algorithm>

// Looking at the structs themselves is not useful if your goal is simply to use the library.
// Jump to after line 165 to see the API functions you want to call.
// Or, look at the examples to see how to use these functions.

namespace flaf {
typedef std::variant<std::function<int(size_t,char**)>,std::function<int(std::vector<std::string>)>> C_or_cpp_function;

struct Flag {
    std::string name;
    std::function<int()> function;
    size_t exactly_this_many;
    std::string short_help;

    Flag();
    Flag(std::string name, std::function<int()> function, size_t exactly_this_many, std::string short_help);
};

struct Example {
    std::string explain;
    std::string flags;
    bool as_root;
    bool tested_yet;

    Example(std::string explain, std::string flags, bool as_root);
};

struct Subcommand {
    std::pair<std::string, std::string> name;
    C_or_cpp_function function;
    std::unordered_map<std::string, Flag> *flags;
    size_t at_least_this_many;
    std::string short_help;
    std::string header_help;
    std::string footer_help;
    std::unordered_map<std::string, Example> *examples;

    Subcommand();

    Subcommand(
        std::pair<std::string, std::string> name,
        std::function<int(std::vector<std::string>)> function,
        size_t at_least_this_many,
        std::string short_help,
        std::string header_help,
        std::string footer_help
    );

    Subcommand(
        std::pair<std::string, std::string> name,
        std::function<int(size_t, char **)> function,
        size_t at_least_this_many,
        std::string short_help,
        std::string header_help,
        std::string footer_help
    );

    Subcommand(
        std::pair<std::string, std::string> name,
        C_or_cpp_function function,
        size_t at_least_this_many,
        std::string short_help,
        std::string header_help,
        std::string footer_help
    );
};

struct Subroot {
    std::string name;
    std::unordered_map<std::string, Subcommand*> *subcommands;
    std::unordered_map<std::string, Flag> *flags;
    std::string short_help;
    std::string header_help;
    std::string footer_help;

    Subroot(
        std::string name,
        std::string short_help,
        std::string header_help,
        std::string footer_help
    );
};

struct Command {
    std::string name;
    C_or_cpp_function function;
    std::unordered_map<std::string, Flag> *flags;
    size_t at_least_this_many;
    std::string short_help;
    std::string header_help;
    std::string footer_help;
    std::unordered_map<std::string, Example> *examples;

    Command();

    Command(
        std::string name,
        std::function<int(std::vector<std::string>)> function,
        size_t at_least_this_many,
        std::string short_help,
        std::string header_help,
        std::string footer_help
    );

    Command(
        std::string name,
        std::function<int(size_t, char **)> function,
        size_t at_least_this_many,
        std::string short_help,
        std::string header_help,
        std::string footer_help
    );

    Command(
        std::string name,
        C_or_cpp_function function,
        size_t at_least_this_many,
        std::string short_help,
        std::string header_help,
        std::string footer_help
    );
};

struct Root {
    bool enabled;
    std::string program_name;
    std::string header_help;
    std::string footer_help;
    std::unordered_map<std::string, Flag> *flags;

    std::unordered_map<std::string, std::variant<Command*, Subroot*>> commands;

    size_t unsorted_arguments_index;
    size_t argc;
    std::vector<std::string> unsorted_arguments;
    std::vector<std::string> arguments;

    Flag *current_flag;
    size_t flag_has_consumed;

    std::string resolved_command_name;
    std::string resolved_subcommand_name;

    Root();

    std::string handle_flags(std::unordered_map<std::string, Flag> *name_to_flag, bool prepared_to_call);

    void call_flag(Flag *f);

    int call_function(C_or_cpp_function call_this, std::vector<std::string> with_this);

    int main(int argc, char *argv[]);
};

// Set name of program and header/footer to show at program root.
// Required before anything else can be set.
void root_set(std::string name, std::string header_help, std::string footer_help);

// Create a command accessed using its name.
// The command calls function_to_call once its flags have been handled.
// It shall expect that at_least_this_many arguments have been passed to it.
// The short_help field is displayed when showing this command among available commands.
// The header_help and footer_help fields display when -h or --help are passed to the command.
//
// One variant expects a function that returns int and takes in a vector of strings
// The other variant expects a function that returns int and takes in a size_t argc and char** argv
void command(std::string name_of_command, std::function<int(std::vector<std::string>)> function_to_call, size_t at_least_this_many, std::string short_help, std::string header_help, std::string footer_help);
void command(std::string name_of_command, std::function<int(size_t, char **)> function_to_call, size_t at_least_this_many, std::string short_help, std::string header_help, std::string footer_help);

// Create a subcommand accessed by its name below the named subroot.
// The subcommand calls function_to_call once its flags have been handled.
// It shall expect that at_least_this_many arguments have been passed to it.
// The short_help field is displayed when showing this subcommand among available subcommands.
// The header_help and footer_help fields display when -h or --help are passed to the subcommand.
//
// One variant expects a function that returns int and takes in a vector of strings
// The other variant expects a function that returns int and takes in a size_t argc and char** argv
void subcommand(std::string name_of_subroot, std::string name_of_subcommand, std::function<int(size_t, char **)> function_to_call, size_t at_least_this_many, std::string short_help, std::string header_help, std::string footer_help);
void subcommand(std::string name_of_subroot, std::string name_of_subcommand, std::function<int(std::vector<std::string>)> function_to_call, size_t at_least_this_many, std::string short_help, std::string header_help, std::string footer_help);

// To create multiple commands below some command name you instead create a subroot.
// The name_of_subroot is the base prefix that should be used.
// The short_help field is displayed when showing this subroot alongside other commands/subroots.
// The header_help and footer_help fields display when -h or --help are passed to the subroot.
//
// Consider: my_cli apply template new_template.tmpl
// In this example, my_cli is the name of the program.
// In this example, apply is the name of the subroot
// In this example, template is the name of the subcommand
void subroot(std::string name_of_subroot, std::string short_help, std::string header_help, std::string footer_help);

// Add a flag below the root of the command (when no arguments are passed).
// When the string_to_look_for flag is encountered, function_to_call is called.
// A number of arguments exactly equal to the field exactly_this_many are expected to be chomped by the flag.
// The field short_help is displayed when passing -h or --help to the program with no command name
//
// Getting the next argument can be accomplished by calling flaf::next_argument or flaf::next_arguments.
// Getting the current number of arguments that will be passed to the command/subcommand can be found with flaf::argc.
void root_flags(std::string string_to_look_for, std::function<int()> function_to_call, size_t exactly_this_many, std::string short_help);

// Add a flag below the named command.
// When the string_to_look_for flag is encountered, function_to_call is called.
// A number of arguments exactly equal to the field exactly_this_many are expected to be chomped by the flag.
// The field short_help is displayed when passing -h or --help flag to the command.
//
// Getting the next argument can be accomplished by calling flaf::next_argument or flaf::next_arguments.
// Getting the current number of arguments that will be passed to the command/subcommand can be found with flaf::argc.
void command_flags(std::string command_name, std::string string_to_look_for, std::function<int()> function_to_call, size_t exactly_this_many, std::string short_help);

// Add a flag below the named subroot.
// When the string_to_look_for flag is encountered, function_to_call is called.
// A number of arguments exactly equal to the field exactly_this_many are expected to be chomped by the flag.
// The field short_help is displayed when passing -h or --help flag to the subroot.
//
// Chomping the next argument can be accomplished by calling flaf::next_argument or flaf::next_arguments.
// Getting the current number of arguments that will be passed to the command/subcommand can be found with flaf::argc.
void subroot_flags(std::string command_name, std::string string_to_look_for, std::function<int()> function_to_call, size_t exactly_this_many, std::string short_help);

// Add a flag below the named subcommand.
// When the string_to_look_for flag is encountered, function_to_call is called.
// A number of arguments exactly equal to the field exactly_this_many are expected to be chomped by the flag.
// The field short_help is displayed when passing -h or --help flag to the subcommand.
//
// Chomping the next argument can be accomplished by calling flaf::next_argument or flaf::next_arguments.
// Getting the current number of arguments that will be passed to the command/subcommand can be found with flaf::argc.
void subcommand_flags(std::string command_name, std::string subcommand_name, std::string string_to_look_for, std::function<int()> function_to_call, size_t exactly_this_many, std::string short_help);

// Register example for command registering under a name.
// Provide a useful_description of what passing the arguments_and_flags presented will do.
// Use as_root to control whether the examples display a user ($) or root (#) console.
void command_example(std::string command_name, std::string example_name, std::string useful_description, std::string arguments_and_flags, bool as_root);

// Register example for subcommand registering under a name.
// Provide a useful_description of what passing the arguments_and_flags presented will do.
// Use as_root to control whether the examples display a user ($) or root (#) console.
void subcommand_example(std::string subroot_name, std::string subcommand_name, std::string example_name,  std::string useful_description, std::string arguments_and_flags, bool as_root);

// Indicate that some command example has been tested
void command_test(std::string command_name, std::string example_name);

// Indicate that some subcommand example has been tested
void subcommand_test(std::string subroot_name, std::string subcommand_name, std::string example_name);

// Print examples that have and have not been tested.
// If exit_with_failure_if_not_done is true then fail when all examples have not been tested.
void test_status(bool exit_with_failure_if_not_done);

// Print debug information
void debug(std::string debug_msg);

// Print warning information
void warning(std::string warn_msg);

// Print error information
void error(std::string error_msg);

// Print something then exit with EXIT_FAILURE
void failure(std::string fail_msg);

// Get current number of arguments planned to pass to command/subcommand.
// Can be useful when using callbacks with flags.
size_t argc();

// Chomp next argument preventing it from being passed to command/subcommand.
// Unlike the C API equivalent flaf_next_argument NEVER free/delete memory returned from here!
std::string next_argument();

// Chomp next n arguments preventing them from being passed to command/subcommand.
// Unlike the C API equivalent flaf_next_argument NEVER free/delete memory returned from here!
std::vector<std::string> next_arguments(size_t n);

// Route to the appropriate command/subcommand based on the user supplied flags/arguments.
int main(int argc, char **argv);

// Everything below here is likely not useful unless you want to modify the library itself
// Everything below here is likely not useful unless you want to modify the library itself
// Everything below here is likely not useful unless you want to modify the library itself

extern struct Root *root;

int help();

int help_root(size_t padding);

int help_command(Command a_command, size_t padding);

int help_subroot(Subroot a_subroot, size_t padding);

int help_subcommand(Subcommand a_subcommand, size_t padding);

namespace helper {
std::vector<std::string> sort_names(std::vector<std::string> strings);

std::vector<std::string> sort_flag_names(std::vector<std::string> flag_names);

size_t get_longest_string_size(std::vector<std::string> strings);

enum stderr_send_kind {
    STDERR_KIND_UNSET,
    STDERR_KIND_INVALID,

    STDERR_KIND_DEBUG,
    STDERR_KIND_WARNING,
    STDERR_KIND_ERROR,
    STDERR_KIND_FAILURE
};

void stderr_send(std::string msg, enum stderr_send_kind send_kind);
}

namespace must {
void ensure_root_is_set();
Command *get_command(std::string command_name);
Subroot *get_subroot(std::string subroot_name);
Subcommand *get_subcommand(Subroot *subroot, std::string subcommand_name);
Flag *get_root_flag(std::string flag_name);
Flag *get_command_flag(Command *command, std::string flag_name);
Flag *get_subroot_flag(Subroot *subroot, std::string flag_name);
Flag *get_subcommand_flag(Subcommand *subcommand, std::string flag_name);
};
}

#include <string>
#include <format>
#include <variant>
#include <functional>
#include <vector>
#include <unordered_map>
#include <cassert>
#include <iostream>
#include <cstdlib>
#include <iomanip>
#include <ranges>

#include <string.h>

#include "flaf.hpp"

// CPP api

namespace flaf {
Flag::Flag() : Flag("", nullptr, 0, "") {}

Flag::Flag(std::string name, std::function<int()> function, size_t exactly_this_many, std::string short_help) :
    name{name},
    function{function},
    exactly_this_many{exactly_this_many},
    short_help{short_help}
{};

Example::Example(std::string explain, std::string flags, bool as_root) : explain{explain}, flags{flags}, as_root{as_root}, tested_yet{false} {};

Subroot::Subroot(
    std::string name,
    std::string short_help,
    std::string header_help,
    std::string footer_help
) :
    name{name},
    short_help{short_help},
    header_help{header_help},
    footer_help{footer_help}
{
    subcommands = new std::unordered_map<std::string, Subcommand*>();
    flags = new std::unordered_map<std::string, Flag>();
};

Subcommand::Subcommand() : Subcommand(std::pair<std::string, std::string>("", ""), std::function<int(size_t, char**)>(nullptr), 0, "", "", "") { };

Subcommand::Subcommand(
    std::pair<std::string, std::string> name,
    std::function<int(std::vector<std::string>)> function,
    size_t at_least_this_many,
    std::string short_help,
    std::string header_help,
    std::string footer_help
) : Subcommand(name, C_or_cpp_function(function), at_least_this_many, short_help, header_help, footer_help) {};

Subcommand::Subcommand(
    std::pair<std::string, std::string> name,
    std::function<int(size_t, char**)> function,
    size_t at_least_this_many,
    std::string short_help,
    std::string header_help,
    std::string footer_help
) : Subcommand(name, C_or_cpp_function(function), at_least_this_many, short_help, header_help, footer_help) { };

Subcommand::Subcommand(
    std::pair<std::string, std::string> name,
    C_or_cpp_function function,
    size_t at_least_this_many,
    std::string short_help,
    std::string header_help,
    std::string footer_help
) :
    name{name},
    function{function},
    at_least_this_many{at_least_this_many},
    short_help{short_help},
    header_help{header_help},
    footer_help{footer_help}
{
    this->flags = new std::unordered_map<std::string, Flag>();
    this->examples = new std::unordered_map<std::string, Example>();
};

Command::Command() : Command("", std::function<int(size_t, char**)>(nullptr), 0, "", "", "") {};

Command::Command(
    std::string name,
    std::function<int(std::vector<std::string>)> function,
    size_t at_least_this_many,
    std::string short_help,
    std::string header_help,
    std::string footer_help
) : Command(name, C_or_cpp_function(function), at_least_this_many, short_help, header_help, footer_help) {};

Command::Command(
    std::string name,
    std::function<int(size_t, char**)> function,
    size_t at_least_this_many,
    std::string short_help,
    std::string header_help,
    std::string footer_help
) : Command(name, C_or_cpp_function(function), at_least_this_many, short_help, header_help, footer_help) {};

Command::Command(
    std::string name,
    C_or_cpp_function function,
    size_t at_least_this_many,
    std::string short_help,
    std::string header_help,
    std::string footer_help
) :
    name{name},
    function{function},
    at_least_this_many{at_least_this_many},
    short_help{short_help},
    header_help{header_help},
    footer_help{footer_help}
{
    this->flags = new std::unordered_map<std::string, Flag>();
    this->examples = new std::unordered_map<std::string, Example>();
};

Root::Root() :
    enabled{false},
    program_name{""},
    header_help{""},
    footer_help{""},
    unsorted_arguments_index{0},
    argc{0},
    current_flag{nullptr},
    flag_has_consumed{0},
    resolved_command_name{""},
    resolved_subcommand_name{""}
{
    this->flags = new std::unordered_map<std::string, Flag>();
    this->commands = std::unordered_map<std::string, std::variant<Command*, Subroot*>>();
    this->unsorted_arguments = std::vector<std::string>();
    this->arguments = std::vector<std::string>();
};

std::string Root::handle_flags(std::unordered_map<std::string, Flag> *some_flags, bool prepared_to_call) {
    std::string return_me = "";
    bool done = false;
    bool stop_processing_flags = false;

    while(!done) {
        if(unsorted_arguments_index >= unsorted_arguments.size()) {
            unsorted_arguments_index++;
            break;
        }

        auto argument = next_argument();
        if((argument[0] == '-') && (argument[1] == '-')) {
            stop_processing_flags = true;
            continue;
        } else if((argument[0] == '-') && !(stop_processing_flags)) {
            if(!some_flags->contains(argument)) {
                [[unlikely]]
                help();
                std::cerr << '\n';
                failure(std::format("Bad flag named {}", argument));
            }
            Flag f = some_flags->at(argument);
            call_flag(&f);
        } else if(prepared_to_call) {
            root->argc++;
            arguments.push_back(argument);
        } else {
            done = true;
            return_me = argument;
        }
    }

    return return_me;
}

void Root::call_flag(Flag *f) {
    this->current_flag = f;
    this->flag_has_consumed = 0;

    int ret = f->function();
    if(ret != 0) {
        std::exit(EXIT_FAILURE);
    }

    if(this->flag_has_consumed != f->exactly_this_many) {
        [[unlikely]]
        help();
        std::cerr << '\n';
        failure(std::format("Flag {} did not consume {} arguments. Instead consumed {}", f->name, f->exactly_this_many, this->flag_has_consumed));
    }
    unsorted_arguments_index += flag_has_consumed;

    this->flag_has_consumed = 0;
    this->current_flag = nullptr;
}

int Root::call_function(C_or_cpp_function call_this, std::vector<std::string> with_this) {
    size_t argc = with_this.size();
    char **argv = new char*[argc];
    for(size_t i = 0; i < argc; i++) {
        argv[i] = strdup(with_this[i].c_str());
    }

    auto *c_function = std::get_if<std::function<int(size_t,char**)>>(&call_this);
    if(c_function != nullptr && *c_function != nullptr) {
        return (*c_function)(argc, argv);
    }
    auto *cpp_function = std::get_if<std::function<int(std::vector<std::string>)>>(&call_this);
    if(cpp_function != nullptr && *cpp_function != nullptr) {
        return (*cpp_function)(with_this);
    }
    return EINVAL;
}

int Root::main(int argc, char *argv[]) {
    if(argc == 0) {
        [[unlikely]]
        failure("Argc is 0. Make sure you pass arguments that are unmodified.");
    }

    for(int i = 1; i < argc; i++) {
        unsorted_arguments.push_back(std::string(argv[i]));
    }

    std::string maybe_command_name = handle_flags(root->flags, false);
    root->argc = 0;
    if(!commands.contains(maybe_command_name)) {
        return help();
    }

    root->resolved_command_name = maybe_command_name;
    auto command_or_subroot = commands[maybe_command_name];
    Command **command_ptr = std::get_if<Command*>(&command_or_subroot);
    Subroot **subroot_ptr = std::get_if<Subroot*>(&command_or_subroot);
    if(command_ptr != nullptr && *command_ptr != nullptr) {
        Command *command = *command_ptr;
        std::unordered_map<std::string, Flag> *flags_belonging_to_command = command->flags;
        auto _ = handle_flags(flags_belonging_to_command, true);
        return call_function(command->function, arguments);
    } else if(subroot_ptr != nullptr && *subroot_ptr != nullptr) {
        Subroot *subroot = *subroot_ptr;
        std::unordered_map<std::string, Flag> *flags_belonging_to_subcommand_root = subroot->flags;
        std::string maybe_subcommand_name = handle_flags(flags_belonging_to_subcommand_root, false);
        root->argc = 0;

        if(subroot->subcommands->contains(maybe_subcommand_name)) {
            root->resolved_subcommand_name = maybe_subcommand_name;
            Subcommand *subcommand = subroot->subcommands->at(maybe_subcommand_name);
            std::unordered_map<std::string, Flag> *flags_belonging_to_subcommand = subcommand->flags;
            auto _ = handle_flags(flags_belonging_to_subcommand, true);
            return call_function(subcommand->function, arguments);
        } else {
            return help();
        }
    }

    return EINVAL;
}

Root *root = new Root();

int help() {
    must::ensure_root_is_set();

    size_t padding = 8;

    if(root->resolved_command_name == std::string("")) {
        return help_root(padding);
    }

    if(!root->commands.contains(root->resolved_command_name)) {
        return help_root(padding);
    }
    std::variant<Command*, Subroot*> command_or_subroot = root->commands.at(root->resolved_command_name);
    Command **as_command_ptr = std::get_if<Command *>(&command_or_subroot);
    Subroot **as_subroot_ptr = std::get_if<Subroot *>(&command_or_subroot);
    if(as_command_ptr) {
        Command *command = must::get_command(root->resolved_command_name);
        return help_command(*command, padding);
    }
    if(as_subroot_ptr) {
        Subroot *subroot = must::get_subroot(root->resolved_command_name);
        if(
            root->resolved_subcommand_name == std::string("") ||
            !subroot->subcommands->contains(root->resolved_subcommand_name)
        ) {
            return help_subroot(*subroot, padding);
        }
        Subcommand *subcommand = must::get_subcommand(subroot, root->resolved_subcommand_name);
        return help_subcommand(*subcommand, padding);
    }

    return EINVAL;
}

int help_root(size_t padding) {
    must::ensure_root_is_set();

    std::string enforced_padding = std::string(padding, ' ');

    auto range_over_command_keys = flaf::root->commands | std::ranges::views::keys;
    auto unsorted_command_names = std::vector<std::string>();
    for(auto key : range_over_command_keys) {
        unsorted_command_names.push_back(key);
    }
    auto sorted_command_names = helper::sort_names(unsorted_command_names);
    size_t longest_command_name_size = helper::get_longest_string_size(sorted_command_names);

    auto range_over_flag_keys = *(flaf::root->flags) | std::ranges::views::keys;
    auto unsorted_flag_names = std::vector<std::string>();
    for(auto key : range_over_flag_keys) {
        unsorted_flag_names.push_back(key);
    }
    auto sorted_flag_names = helper::sort_flag_names(unsorted_flag_names);
    size_t longest_flag_name_size = helper::get_longest_string_size(sorted_flag_names);

    size_t longest_command_or_flag_name_size = std::max(longest_command_name_size, longest_flag_name_size);

    std::cerr << std::format("{}\n", root->program_name);
    std::cerr << std::format("\n");
    std::cerr << std::format("{}\n", root->header_help);

    std::cerr << "Commands:\n";
    for(const std::string &command_name : sorted_command_names) {
        size_t current_command_name_size = command_name.size();
        std::string uniform_length_padding = std::string(longest_command_or_flag_name_size - current_command_name_size, ' ');
        std::string padding = std::format("{}{}", uniform_length_padding, enforced_padding);

        auto variant = root->commands.at(command_name);
        Command **command_ptr = std::get_if<Command*>(&variant);
        Subroot **subroot_ptr = std::get_if<Subroot*>(&variant);

        if(command_ptr != nullptr && *command_ptr != nullptr) {
            Command *command = *command_ptr;
            const std::string command_name = command->name;
            const std::string command_help = command->short_help;
            std::cerr << std::format("  {}{}{}\n", command_name, padding, command_help);
        } else if(subroot_ptr != nullptr && *subroot_ptr != nullptr) {
            Subroot *subroot = *subroot_ptr;
            const std::string subroot_name = subroot->name;
            const std::string subroot_help = subroot->short_help;
            std::cerr << std::format("  {}{}{}\n", subroot_name, padding, subroot_help);
        }
    }

    std::cerr << std::format("\n");

    std::cerr << "Flags:\n";
    for(const std::string &flag_name : sorted_flag_names) {
        size_t current_flag_name_size = flag_name.size();
        std::string uniform_length_padding = std::string(longest_command_or_flag_name_size - current_flag_name_size, ' ');
        std::string padding = std::format("{}{}", uniform_length_padding, enforced_padding);

        const std::string flag_help = root->flags->at(flag_name).short_help;
        std::cerr << std::format("  {}{}{}\n", flag_name, padding, flag_help);
    }

    std::cerr << std::format("\n{}", root->footer_help);

    return EINVAL;
}

int help_command(Command a_command, size_t padding) {
    must::ensure_root_is_set();

    std::string enforced_padding = std::string(padding, ' ');

    auto range_over_flag_keys = *(a_command.flags) | std::ranges::views::keys;
    auto unsorted_flag_names = std::vector<std::string>();
    for(auto key : range_over_flag_keys) {
        unsorted_flag_names.push_back(key);
    }
    auto sorted_flag_names = helper::sort_flag_names(unsorted_flag_names);
    size_t longest_flag_name_size = helper::get_longest_string_size(sorted_flag_names);

    std::cerr << std::format("{}\n", a_command.short_help);
    std::cerr << std::format("\n");
    std::cerr << std::format("{}\n", a_command.header_help);

    std::cerr << "Flags:\n";
    for(auto flag_name : sorted_flag_names) {
        size_t current_flag_name_size = flag_name.size();
        std::string uniform_length_padding = std::string(longest_flag_name_size - current_flag_name_size, ' ');
        std::string padding = std::format("{}{}", uniform_length_padding, enforced_padding);

        const std::string flag_help = a_command.flags->at(flag_name).short_help;
        std::cerr << std::format("  {}{}{}\n", flag_name, padding, flag_help);
    }

    std::cerr << "\n";

    std::cerr << "Examples:\n";
    bool first = true;
    for(auto explanation_example : *(a_command.examples)) {
        if(first) {
            first = false;
        } else {
            std::cerr << '\n';
        }

        std::cerr << std::format("  #{}\n", explanation_example.second.explain);
        if(explanation_example.second.as_root) {
            std::cerr << std::format("  (root) # {} {} {}\n", root->program_name, a_command.name, explanation_example.second.flags);
        } else {
            std::cerr << std::format("  $ {} {} {}\n", root->program_name, a_command.name, explanation_example.second.flags);
        }
    }

    std::cerr << std::format("\n{}", a_command.footer_help);

    return EINVAL;
}

int help_subroot(Subroot a_subroot, size_t padding) {
    must::ensure_root_is_set();

    std::string enforced_padding = std::string(padding, ' ');

    auto range_over_subcommand_keys = *(a_subroot.subcommands) | std::ranges::views::keys;
    auto unsorted_subcommand_names = std::vector<std::string>();
    for(auto key : range_over_subcommand_keys) {
        unsorted_subcommand_names.push_back(key);
    }
    auto sorted_subcommand_names = helper::sort_names(unsorted_subcommand_names);
    size_t longest_subcommand_name_size = helper::get_longest_string_size(sorted_subcommand_names);

    auto range_over_flag_keys = *(a_subroot.flags) | std::ranges::views::keys;
    auto unsorted_flag_names = std::vector<std::string>();
    for(auto key : range_over_flag_keys) {
        unsorted_flag_names.push_back(key);
    }
    auto sorted_flag_names = helper::sort_flag_names(unsorted_flag_names);
    size_t longest_flag_name_size = helper::get_longest_string_size(sorted_flag_names);

    size_t longest_subcommand_or_flag_name_size = std::max(longest_subcommand_name_size, longest_flag_name_size);

    std::cerr << std::format("{}\n", a_subroot.short_help);
    std::cerr << "\n";
    std::cerr << std::format("{}\n", a_subroot.header_help);

    std::cerr << "Commands:\n";
    for(const std::string &subcommand_name : sorted_subcommand_names) {
        size_t current_subcommand_name_size = subcommand_name.size();
        std::string uniform_length_padding = std::string(longest_subcommand_or_flag_name_size - current_subcommand_name_size, ' ');
        std::string padding = std::format("{}{}", uniform_length_padding, enforced_padding);

        Subcommand *subcommand = a_subroot.subcommands->at(subcommand_name);
        std::cerr << std::format("  {}{}{}\n", subcommand->name.second, padding, subcommand->short_help);
    }

    std::cerr << std::format("\n");

    std::cerr << "Flags:\n";
    for(const std::string &flag_name : sorted_flag_names) {
        size_t current_flag_name_size = flag_name.size();
        std::string uniform_length_padding = std::string(longest_subcommand_or_flag_name_size - current_flag_name_size, ' ');
        std::string padding = std::format("{}{}", uniform_length_padding, enforced_padding);

        const std::string flag_help = a_subroot.flags->at(flag_name).short_help;
        std::cerr << std::format("  {}{}{}\n", flag_name, padding, flag_help);
    }

    std::cerr << std::format("\n{}", a_subroot.footer_help);

    return EINVAL;
}

int help_subcommand(Subcommand a_subcommand, size_t padding) {
    must::ensure_root_is_set();

    std::string enforced_padding = std::string(padding, ' ');

    auto range_over_flag_keys = *(a_subcommand.flags) | std::ranges::views::keys;
    auto unsorted_flag_names = std::vector<std::string>();
    for(auto key : range_over_flag_keys) {
        unsorted_flag_names.push_back(key);
    }
    auto sorted_flag_names = helper::sort_flag_names(unsorted_flag_names);
    size_t longest_flag_name_size = helper::get_longest_string_size(sorted_flag_names);

    std::cerr << std::format("{}\n\n", a_subcommand.short_help);

    std::cerr << std::format("{}\n", a_subcommand.header_help);

    std::cerr << "Flags:\n";
    for(const auto &flag_name : sorted_flag_names) {
        size_t current_flag_name_size = flag_name.size();
        std::string uniform_length_padding = std::string(longest_flag_name_size - current_flag_name_size, ' ');
        std::string padding = std::format("{}{}", uniform_length_padding, enforced_padding);

        Flag *flag = must::get_subcommand_flag(&a_subcommand, flag_name);
        const std::string flag_help = flag->short_help;
        std::cerr << std::format("  {}{}{}\n", flag_name, padding, flag_help);
    }

    std::cerr << '\n';

    std::cerr << "Examples:\n";
    bool first = true;
    for(auto explanation_example : *(a_subcommand.examples)) {
        if(first) {
            first = false;
        } else {
            std::cerr << '\n';
        }

        std::cerr << std::format("  # {}\n", explanation_example.second.explain);
        if(explanation_example.second.as_root) {
            std::cerr << std::format("  (root) # {} {} {} {}\n", root->program_name, a_subcommand.name.first, a_subcommand.name.second, explanation_example.second.flags);
        } else {
            std::cerr << std::format("  $ {} {} {} {}\n", root->program_name, a_subcommand.name.first, a_subcommand.name.second, explanation_example.second.flags);
        }
    }

    std::cerr << std::format("\n{}", a_subcommand.footer_help);

    return EINVAL;
}

void root_set(std::string name, std::string header_help, std::string footer_help) {
    root->enabled = true;
    root->program_name = name;
    root->header_help = header_help;
    root->footer_help = footer_help;

    flaf::root_flags("-h", help, 0, "See help");
    flaf::root_flags("--help", help, 0, "See help");
}

void root_flags(std::string string_to_look_for, std::function<int()> function_to_call, size_t exactly_this_many, std::string short_help) {
    must::ensure_root_is_set();
    if(string_to_look_for.size() == 0 || string_to_look_for[0] != '-') {
        [[unlikely]]
        failure("Flag must begin with '-'");
    }

    Flag f = Flag(string_to_look_for, function_to_call, exactly_this_many, short_help);
    root->flags->insert({string_to_look_for, f});
}

void command(std::string name_of_command, C_or_cpp_function function_to_call, size_t at_least_this_many, std::string short_help, std::string header_help, std::string footer_help) {
    must::ensure_root_is_set();
    if(root->commands.contains(name_of_command)) {
        [[unlikely]]
        failure(std::format("Command {} already exists. cannot create new command with same name!", name_of_command));
    }

    root->commands[name_of_command] = new Command(name_of_command, function_to_call, at_least_this_many, short_help, header_help, footer_help);
    command_flags(name_of_command, "-h", help, 0, "See help");
    command_flags(name_of_command, "--help", help, 0, "See help");
}

void command(std::string name_of_command, std::function<int(std::vector<std::string>)> function_to_call, size_t at_least_this_many, std::string short_help, std::string header_help, std::string footer_help) {
    return command(name_of_command, C_or_cpp_function(function_to_call), at_least_this_many, short_help, header_help, footer_help);
}

void command(std::string name_of_command, std::function<int(size_t, char**)> function_to_call, size_t at_least_this_many, std::string short_help, std::string header_help, std::string footer_help) {
    return command(name_of_command, C_or_cpp_function(function_to_call), at_least_this_many, short_help, header_help, footer_help);
}

void command_flags(std::string command_name, std::string string_to_look_for, std::function<int()> function_to_call, size_t exactly_this_many, std::string short_help) {
    must::ensure_root_is_set();
    Command *command = must::get_command(command_name);

    Flag f = Flag(string_to_look_for, function_to_call, exactly_this_many, short_help);
    command->flags->insert({string_to_look_for, f});
}

void subcommand(std::string name_of_subroot, std::string name_of_subcommand, C_or_cpp_function function_to_call, size_t at_least_this_many, std::string short_help, std::string header_help, std::string footer_help) {
    must::ensure_root_is_set();
    Subroot *subroot = must::get_subroot(name_of_subroot);
    if(subroot->subcommands->contains(name_of_subcommand)) {
        failure(std::format("Subcommand {} already exists. cannot create new command with same name!", name_of_subcommand));
    }

    auto name_of_subroot_and_subcommand = std::make_pair(name_of_subroot, name_of_subcommand);
    Subcommand *subcommand = new Subcommand(name_of_subroot_and_subcommand, function_to_call, at_least_this_many, short_help, header_help, footer_help);
    std::unordered_map<std::string, Subcommand*> *subcommands_of_subcommand = subroot->subcommands;
    subcommands_of_subcommand->insert({name_of_subcommand, subcommand});
    subcommand_flags(name_of_subroot, name_of_subcommand, "-h", help, 0, "See help");
    subcommand_flags(name_of_subroot, name_of_subcommand, "--help", help, 0, "See help");
}

void subcommand(std::string name_of_subroot, std::string name_of_subcommand, std::function<int(std::vector<std::string>)> function_to_call, size_t at_least_this_many, std::string short_help, std::string header_help, std::string footer_help) {
    return subcommand(name_of_subroot, name_of_subcommand, C_or_cpp_function(function_to_call), at_least_this_many, short_help, header_help, footer_help);
}

void subcommand(std::string name_of_subroot, std::string name_of_subcommand, std::function<int(size_t, char**)> function_to_call, size_t at_least_this_many, std::string short_help, std::string header_help, std::string footer_help) {
    return subcommand(name_of_subroot, name_of_subcommand, C_or_cpp_function(function_to_call), at_least_this_many, short_help, header_help, footer_help);
}

void subroot(std::string name_of_subroot, std::string short_help, std::string header_help, std::string footer_help) {
    must::ensure_root_is_set();
    if(root->commands.contains(name_of_subroot)) {
        failure(std::format("Subroot {} already exists. cannot create new command with same name!", name_of_subroot));
    }

    Subroot *created_here = new Subroot(name_of_subroot, short_help, header_help, footer_help);
    root->commands[name_of_subroot] = std::variant<Command*, Subroot*>(created_here);
    flaf::subroot_flags(name_of_subroot, "-h", help, 0, "See help");
    flaf::subroot_flags(name_of_subroot, "--help", help, 0, "See help");
}

void subroot_flags(std::string command_name, std::string string_to_look_for, std::function<int()> function_to_call, size_t exactly_this_many, std::string short_help) {
    must::ensure_root_is_set();
    if(string_to_look_for.size() == 0 || string_to_look_for[0] != '-') {
        [[unlikely]]
        failure("Flag must begin with '-'");
    }

    Subroot *subroot = must::get_subroot(command_name);
    Flag f = Flag(string_to_look_for, function_to_call, exactly_this_many, short_help);
    subroot->flags->insert({string_to_look_for, f});
}

void subcommand_flags(std::string command_name, std::string subcommand_name, std::string string_to_look_for, std::function<int()> function_to_call, size_t exactly_this_many, std::string short_help) {
    must::ensure_root_is_set();
    if(string_to_look_for.size() == 0 || string_to_look_for[0] != '-') {
        [[unlikely]]
        failure("Flag must begin with '-'");
    }

    Subroot *subroot = must::get_subroot(command_name);
    Subcommand *subcommand = must::get_subcommand(subroot, subcommand_name);
    Flag f = Flag(string_to_look_for, function_to_call, exactly_this_many, short_help);
    (*subcommand->flags)[string_to_look_for] = f;
}

void command_example(std::string command_name, std::string example_name, std::string useful_description, std::string arguments_and_flags, bool as_root) {
    must::ensure_root_is_set();
    Command *command = must::get_command(command_name);

    auto my_example = Example(useful_description, arguments_and_flags, as_root);
    command->examples->insert({example_name, my_example});
}

void subcommand_example(std::string subroot_name, std::string subcommand_name, std::string example_name,  std::string useful_description, std::string arguments_and_flags, bool as_root) {
    must::ensure_root_is_set();
    Subroot *subroot = must::get_subroot(subroot_name);
    Subcommand *subcommand = must::get_subcommand(subroot, subcommand_name);

    auto my_example = Example(useful_description, arguments_and_flags, as_root);
    subcommand->examples->insert({example_name, my_example});
}

void command_test(std::string command_name, std::string example_name) {
    must::ensure_root_is_set();
    Command *command = must::get_command(command_name);
    if(!command->examples->contains(example_name)) {
        [[unlikely]]
        failure(std::format("No example named \"{}\" under {}", example_name, command_name));
    }
    command->examples->at(example_name).tested_yet = true;
}

void subcommand_test(std::string subroot_name, std::string subcommand_name, std::string example_name) {
    must::ensure_root_is_set();
    Subroot *subroot = must::get_subroot(subroot_name);
    Subcommand *subcommand = must::get_subcommand(subroot, subcommand_name);
    if(!subcommand->examples->contains(example_name)) {
        failure(std::format("No example named \"{}\" under {} part of subroot {}", example_name, subcommand_name, subroot_name));
    }
    subcommand->examples->at(example_name).tested_yet = true;
}

void test_status(bool exit_with_failure_if_not_done) {
    must::ensure_root_is_set();
    bool all_tested = true;

    std::cout << std::format("tests:\n");
    for(auto name_variant : root->commands) {
        std::string command_name = name_variant.first;
        std::string subroot_name = name_variant.first;
        std::variant<Command *, Subroot *> command_or_subroot = name_variant.second;

        Command **command_ptr = std::get_if<Command*>(&command_or_subroot);
        if(command_ptr != nullptr && *command_ptr != nullptr) {
            Command *command = *command_ptr;
            std::cout << std::format("  {}:\n", command_name);
            for(auto name_example : *(command->examples)) {
                std::string example_name = name_example.first;
                Example example = name_example.second;
                all_tested = all_tested && example.tested_yet;

                if(example.tested_yet) {
                    std::cout << std::format("    {}_{}: \"TESTED!\"\n", command_name, example_name);
                } else {
                    std::cout << std::format("    {}_{}: \"NOT TESTED!\"\n", command_name, example_name);
                }
            }
        }

        Subroot **subroot_ptr = std::get_if<Subroot*>(&command_or_subroot);
        if(subroot_ptr != nullptr && *subroot_ptr != nullptr) {
            Subroot *subroot = *subroot_ptr;
            std::cout << std::format("  {}:\n", subroot_name);
            for(auto name_subcommand : *(subroot->subcommands)) {
                std::string subcommand_name = name_subcommand.first;
                Subcommand *subcommand = name_subcommand.second;
                std::cout << std::format("    {}_{}:\n", subroot_name, subcommand_name);
                for(auto name_example : *(subcommand->examples)) {
                    std::string example_name = name_example.first;
                    Example example = name_example.second;
                    all_tested = all_tested && example.tested_yet;

                    if(example.tested_yet) {
                        std::cout << std::format("      {}_{}_{}: \"TESTED!\"\n", subroot_name, subcommand_name, example_name);
                    } else {
                        std::cout << std::format("      {}_{}_{}: \"NOT TESTED!\"\n", subroot_name, subcommand_name, example_name);
                    }
                }
            }
        }
    }

    if(!all_tested && exit_with_failure_if_not_done) {
        failure("All not tested!");
    }
}

size_t argc() {
    must::ensure_root_is_set();
    return root->argc;
}

std::string next_argument() {
    must::ensure_root_is_set();

    std::string return_me;
    if(root->unsorted_arguments_index >= root->unsorted_arguments.size()) {
        [[unlikely]]
        help();
        std::cerr << '\n';
        failure("No next argument to flag/command");
    }

    size_t index = root->unsorted_arguments_index + root->flag_has_consumed;
    if(root->current_flag == nullptr) {
        return_me = root->unsorted_arguments[root->unsorted_arguments_index++];
    } else {
        root->argc++;
        root->flag_has_consumed++;
        return_me = root->unsorted_arguments[index];
    }

    return return_me;
}

std::vector<std::string> next_arguments(size_t n) {
    must::ensure_root_is_set();

    auto return_this = std::vector<std::string>();
    for(size_t i = 0; i < n; i++) {
        return_this.push_back(next_argument());
    }

    return return_this;
}

int main(int argc, char **argv) {
    must::ensure_root_is_set();

    return root->main(argc, argv);
}

void debug(std::string debug_msg) {
    return helper::stderr_send(debug_msg, helper::STDERR_KIND_DEBUG);
}

void warning(std::string warning_msg) {
    return helper::stderr_send(warning_msg, helper::STDERR_KIND_WARNING);
}

void error(std::string error_msg) {
    return helper::stderr_send(error_msg, helper::STDERR_KIND_ERROR);
}

void failure(std::string failure_msg) {
    helper::stderr_send(failure_msg, helper::STDERR_KIND_FAILURE);
    return std::exit(EXIT_FAILURE);
}

namespace helper {
std::vector<std::string> sort_names(std::vector<std::string> strings) {
    std::sort(strings.begin(), strings.end(), [](std::string a, std::string b) {
        return a < b;
    });
    return strings;
}

std::vector<std::string> sort_flag_names(std::vector<std::string> flag_names) {
    auto return_these_names = std::vector<std::string>();

    // matched by -.$
    auto short_names = std::vector<std::string>();

    // matched by -..*
    auto shortish_names = std::vector<std::string>();

    // matched by --.*
    auto long_names = std::vector<std::string>();

    for(auto flag_name : flag_names) {
        if(flag_name[1] == '-') {
            long_names.push_back(flag_name);
        } else if (flag_name.size() > 2) {
            shortish_names.push_back(flag_name);
        } else {
            short_names.push_back(flag_name);
        }
    }

    std::sort(short_names.begin(), short_names.end(), [](std::string a, std::string b) {
        return a < b;
    });
    std::sort(shortish_names.begin(), shortish_names.end(), [](std::string a, std::string b) {
        return a < b;
    });
    std::sort(long_names.begin(), long_names.end(), [](std::string a, std::string b) {
        return a < b;
    });

    return_these_names.insert(return_these_names.end(), short_names.begin(), short_names.end());
    return_these_names.insert(return_these_names.end(), shortish_names.begin(), shortish_names.end());
    return_these_names.insert(return_these_names.end(), long_names.begin(), long_names.end());
    return return_these_names;
}

size_t get_longest_string_size(std::vector<std::string> strings) {
    size_t return_this_length = 0;

    for(auto string : strings) {
        if(string.size() > return_this_length) {
            return_this_length = string.size();
        }
    }

    return return_this_length;
}

void stderr_send(std::string msg, enum stderr_send_kind send_kind) {
    std::string header_string = "";
    switch(send_kind) {
    case STDERR_KIND_DEBUG:
        header_string = "DEBUG";
        break;
    case STDERR_KIND_WARNING:
        header_string = "WARNING";
        break;
    case STDERR_KIND_ERROR:
        header_string = "ERROR";
        break;
    case STDERR_KIND_FAILURE:
        header_string = "FAILURE";
        break;
    default:
        assert(false);
        break;
    }

    std::cerr << std::format("{}:\n", header_string);
    std::cerr << std::format("  {}\n", msg);
}
};

namespace must {
void ensure_root_is_set() {
    if(!root->enabled) {
        [[unlikely]]
        failure("Root not set! Try initializing with flaf::root_set()");
    }
}

Command *get_command(std::string command_name) {
    if(!root->commands.contains(command_name)) {
        [[unlikely]]
        failure(std::format("No command named {}", command_name));
    }

    std::variant<Command*, Subroot*> variant = root->commands.at(command_name);
    Command **command_ptr = std::get_if<Command*>(&variant);
    if(command_ptr == nullptr) {
        [[unlikely]]
        failure(std::format("Found something not a command at {}. Is {} a subroot?", command_name, command_name));
    }
    return *command_ptr;
}

Subroot *get_subroot(std::string subroot_name) {
    if(!root->commands.contains(subroot_name)) {
        [[unlikely]]
        failure(std::format("No subroot named {}", subroot_name));
    }

    std::variant<Command*, Subroot*> variant = root->commands.at(subroot_name);
    Subroot **subroot_ptr = std::get_if<Subroot*>(&variant);
    if(subroot_ptr == nullptr) {
        [[unlikely]]
        failure(std::format("Found something not a subroot at {}. Is {} a command?", subroot_name, subroot_name));
    }

    return *subroot_ptr;
}

Subcommand *get_subcommand(Subroot *subroot, std::string subcommand_name) {
    if(!subroot->subcommands->contains(subcommand_name)) {
        [[unlikely]]
        failure(std::format("No subcommand named {} under {}", subcommand_name, subroot->name));
    }

    return subroot->subcommands->at(subcommand_name);
}

Flag *get_root_flag(std::string flag_name) {
    if(!root->flags->contains(flag_name)) {
        [[unlikely]]
        failure(std::format("No flag named {}", flag_name));
    }

    return &(root->flags->at(flag_name));
}

Flag *get_command_flag(Command *command, std::string flag_name) {
    if(!command->flags->contains(flag_name)) {
        [[unlikely]]
        failure(std::format("No flag named {} under {}", flag_name, command->name));
    }

    return &(command->flags->at(flag_name));
}

Flag *get_subroot_flag(Subroot *subroot, std::string flag_name) {
    if(!subroot->flags->contains(flag_name)) {
        [[unlikely]]
        failure(std::format("No flag named {} under {}", flag_name, subroot->name));
    }

    return &(subroot->flags->at(flag_name));
}

Flag *get_subcommand_flag(Subcommand *subcommand, std::string flag_name) {
    if(!subcommand->flags->contains(flag_name)) {
        [[unlikely]]
        failure(std::format("No subroot named {} under {}", flag_name, subcommand->name.second));
    }

    return &(subcommand->flags->at(flag_name));
}
};
};

// C api

extern "C" void flaf_root_set(char *name, char *header_help, char *footer_help) {
    return flaf::root_set(name, header_help, footer_help);
}

extern "C" void flaf_command(char *name_of_command, int (*function_to_call)(size_t, char **), size_t at_least_this_many, char *short_help, char *header_help, char *footer_help) {
    return flaf::command(name_of_command, flaf::C_or_cpp_function(function_to_call), at_least_this_many, short_help, header_help, footer_help);
}

extern "C" void flaf_subcommand(char *name_of_subroot, char *name_of_subcommand, int (*function_to_call)(size_t, char **), size_t at_least_this_many, char *short_help, char *header_help, char *footer_help) {
    return flaf::subcommand(name_of_subroot, name_of_subcommand, flaf::C_or_cpp_function(function_to_call), at_least_this_many, short_help, header_help, footer_help);
}

extern "C" void flaf_subroot(char *name_of_subroot, char *short_help, char *header_help, char *footer_help) {
    return flaf::subroot(name_of_subroot, short_help, header_help, footer_help);
}

extern "C" void flaf_root_flags(char *string_to_look_for, int (*function_to_call)(), size_t exactly_this_many, char *short_help) {
    return flaf::root_flags(string_to_look_for, function_to_call, exactly_this_many, short_help);
}

extern "C" void flaf_command_flags(char *command_name, char *string_to_look_for, int (*function_to_call)(), size_t exactly_this_many, char *short_help) {
    return flaf::command_flags(command_name, string_to_look_for, function_to_call, exactly_this_many, short_help);
}

extern "C" void flaf_subroot_flags(char *command_name, char *string_to_look_for, int (*function_to_call)(), size_t exactly_this_many, char *short_help) {
    return flaf::subroot_flags(command_name, string_to_look_for, function_to_call, exactly_this_many, short_help);
}

extern "C" void flaf_subcommand_flags(char *command_name, char *subcommand_name, char *string_to_look_for, int (*function_to_call)(), size_t exactly_this_many, char *short_help) {
    return flaf::subcommand_flags(command_name, subcommand_name, string_to_look_for, function_to_call, exactly_this_many, short_help);
}

extern "C" void flaf_command_example(char *command_name, char *example_name, char *useful_description, char *arguments_and_flags, bool as_root) {
    return flaf::command_example(command_name, example_name, useful_description, arguments_and_flags, as_root);
}

extern "C" void flaf_subcommand_example(char *subroot_name, char *subcommand_name, char *example_name,  char *useful_description, char *arguments_and_flags, bool as_root) {
    return flaf::subcommand_example(subroot_name, subcommand_name, example_name, useful_description, arguments_and_flags, as_root);
}

// flaf_mark command example as having been tested.
extern "C" void flaf_command_test(char *command_name, char *example_name) {
    return flaf::command_test(command_name, example_name);
}

// mark subcommand example as having been tested.
extern "C" void flaf_subcommand_test(char *subroot_name, char *subcommand_name, char *example_name) {
    return flaf::subcommand_test(subroot_name, subcommand_name, example_name);
}

// print what examples have and have not been tested
extern "C" void flaf_test_status(bool exit_with_failure_if_not_done) {
    return flaf::test_status(exit_with_failure_if_not_done);
}

extern "C" void flaf_debug(char *debug_msg) {
    return flaf::debug(debug_msg);
}

extern "C" void flaf_warning(char *warn_msg) {
    return flaf::warning(warn_msg);
}

extern "C" void flaf_error(char *error_msg) {
    return flaf::error(error_msg);
}

extern "C" void flaf_failure(char *fail_msg) {
    return flaf::failure(fail_msg);
}

extern "C" size_t flaf_argc() {
    return flaf::argc();
}

extern "C" char *flaf_next_argument() {
    std::string cpp_string = flaf::next_argument();
    return strdup(cpp_string.c_str());
}

extern "C" char **flaf_next_arguments(size_t n) {
    std::vector<std::string> cpp_strings = flaf::next_arguments(n);
    assert(cpp_strings.size() == n);

    char **c_strings = new char*[n];
    for(size_t i = 0; i < n; i++) {
        c_strings[i] = strdup(cpp_strings[i].c_str());
    }

    return c_strings;
}

extern "C" int flaf_main(int argc, char **argv) {
    return flaf::main(argc, argv);
}
