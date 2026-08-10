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

#include "ftxui/component/app.hpp"
#include "ftxui/component/captured_mouse.hpp"
#include "ftxui/component/component.hpp"
#include "ftxui/component/component_base.hpp"
#include "ftxui/component/component_options.hpp"
#include "ftxui/dom/elements.hpp"
#include "ftxui/screen/color.hpp"

namespace ui {
inline std::optional<std::string> respond(const std::string &question) {
    auto screen = ftxui::App::TerminalOutput();

    bool selection_confirmed = false;
    std::string response("");

    ftxui::InputOption input_option{};
    input_option.multiline = false;
    input_option.on_enter = [&] {
        selection_confirmed = true;
        screen.ExitLoopClosure()();
    };
    auto input = ftxui::Input(&response, "", input_option);

    auto container = ftxui::Container::Vertical({
        input
    });

    auto renderer = ftxui::Renderer(container, [&] {
        return ftxui::vbox({
            ftxui::text(question),
            input->Render()
        }) | ftxui::border;
    });

    renderer = ftxui::CatchEvent(renderer, [&](ftxui::Event event) {
        if (event == ftxui::Event::Escape) {
            selection_confirmed = false;
            screen.ExitLoopClosure()();
            return true;
        }

        return false;
    });

    // Important to flush after the loop if you want to write anything else
    screen.Loop(renderer);
    std::cout.flush();
    std::cerr.flush();

    std::optional<std::string> return_value = std::nullopt;
    if(selection_confirmed) {
        return_value = response;
    }
    return return_value;
}

inline std::optional<std::string> fuzzy_select(std::vector<std::string> &options) {
    auto screen = ftxui::App::TerminalOutput();

    bool selection_confirmed = false;
    int selected = 0;
    std::string query("");

    ftxui::InputOption input_option;
    input_option.multiline = false;
    input_option.on_enter = [&] {
        selected = 0;                 // choose first visible item
        selection_confirmed = true;
        screen.ExitLoopClosure()();
    };
    input_option.on_change = [&] {
        helper::fuzzy_sort(options, query);
        selected = 0;
    };
    auto input = ftxui::Input(&query, "search...", input_option);

    ftxui::MenuOption menu_option;
    menu_option.on_enter = [&] {
        selection_confirmed = true;
        screen.ExitLoopClosure()();
    };
    auto menu = ftxui::Menu(&options, &selected, menu_option);

    auto container = ftxui::Container::Vertical({
        input,
        menu,
    });

    auto renderer = ftxui::Renderer(container, [&] {
        return ftxui::vbox({
            ftxui::hbox(
            ftxui::text("Search: "),
            input->Render()
            ),
            ftxui::separator(),
            menu->Render()
            | ftxui::frame
            | ftxui::size(ftxui::HEIGHT, ftxui::LESS_THAN, 10),
            ftxui::separator(),
            ftxui::text("↑↓ move   Enter select   Esc cancel"),
        }) | ftxui::border;
    });

    renderer = ftxui::CatchEvent(renderer, [&](ftxui::Event event) {
        if (event == ftxui::Event::Escape) {
            selection_confirmed = false;
            screen.ExitLoopClosure()();
            return true;
        }

        return false;
    });

    // Important to flush after the loop if you want to write anything else
    screen.Loop(renderer);
    std::cout.flush();
    std::cerr.flush();

    std::optional<std::string> return_value = std::nullopt;
    if(selection_confirmed) {
        return_value = options.at(selected);
    }
    return return_value;
}

inline std::optional<bool> yes_no_prompt(const std::string& question) {
    auto screen = ftxui::App::TerminalOutput();

    std::vector<std::string> choices = {
        "Yes",
        "No",
    };

    int selected = 0;
    std::optional<bool> result;

    ftxui::MenuOption option;
    option.on_enter = [&] {
        result = (selected == 0);
        screen.ExitLoopClosure()();
    };

    auto menu = ftxui::Menu(&choices, &selected, option);

    auto renderer = ftxui::Renderer(menu, [&] {
        return ftxui::vbox({
            ftxui::text(question),
            ftxui::separator(),
            menu->Render(),
        }) | ftxui::border;
    });

    renderer = ftxui::CatchEvent(renderer, [&](ftxui::Event event) {
        if (event == ftxui::Event::Escape) {
            result = std::nullopt;
            screen.ExitLoopClosure()();
            return true;
        }

        return false;
    });

    screen.Loop(renderer);

    return result;
}
}
