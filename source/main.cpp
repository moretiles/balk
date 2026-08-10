#include <iostream>
#include <vector>
#include <unordered_map>
#include <string>
#include <filesystem>
#include <fstream>
#include <cstdlib>
#include <optional>
#include <charconv>
#include <format>

#include <nlohmann/json.hpp>
using json = nlohmann::json;

#include <nlohmann/json-schema.hpp>
using json_validator = nlohmann::json_schema::json_validator;

#include <ersatz.hpp>
#include <cli.hpp>
#include <config.hpp>
#include <ui.hpp>
#include <globals.hpp>

import Constants;

// define instances of types marked extern in globals.hpp
globals::Globals *g = nullptr;

int main(int argc, char *argv[]) {
    // decode timestamp
    /*
    {
        const auto timestamp = helper::get_timestamp_for_directory_and_children(".");
        std::cout << std::format("File write time is {}!\n", *timestamp);
    }
    */

    g = new globals::Globals();

    return cli::main(argc, argv);
}
