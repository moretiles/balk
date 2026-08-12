#pragma once

#include <vector>
#include <unordered_map>
#include <string>
#include <optional>

#include <nlohmann/json.hpp>
using json = nlohmann::json;

#include <nlohmann/json-schema.hpp>
using json_validator = nlohmann::json_schema::json_validator;

namespace config {
struct UnchangableSettings {
    std::string project = "";
    std::string language = "";
    std::string meta_build_system = "";
};

struct Cmake {
    std::string path = "";
    std::vector<std::string> configure_arguments = {};
    std::vector<std::string> build_arguments = {};
};

struct Settings {
    std::string binary = "";
    std::string build_system = "";
    std::string toolchain = "";
    std::string formatter = "";
    std::string linter = "";
    std::string default_profile = "";

    Cmake cmake{};
};

struct OverrideSettings {
    std::string binary = "";
    std::string build_system = "";
    std::string toolchain = "";
    std::string formatter = "";
    std::string linter = "";

    Cmake cmake{};
};

struct Directories {
    std::string balk_path = "";
    std::string build_path = "";
    std::string build_benchmarks_path = "";
    std::string build_examples_path = "";
    std::string build_tests_path = "";
    std::string cmake_path = "";
    std::string examples_path = "";
    std::string export_path = "";
    std::string headers_path = "";
    std::string source_path = "";
    std::string tests_path = "";
    std::string benchmarks_path = "";
};

struct FormatProgram {
    std::string binary = "";
    std::vector<std::string> preappended_arguments = {};
    std::vector<std::string> appended_arguments = {};
};

struct LintProgram {
    std::string binary = "";
    std::vector<std::string> preappended_arguments = {};
    std::vector<std::string> appended_arguments = {};
};

struct BuildSystem {
    std::string build_type = "";
    std::string path = "";
};

struct Toolchain {
    std::string linker = "";
    std::string assembler = "";
    std::string c_compiler = "";
    std::string cpp_compiler = "";
};

struct Command {
    std::string binary = "";
    std::vector<std::string> preappended_arguments = {};
    std::vector<std::string> appended_arguments = {};
};

struct ProfileEntry {
    std::string path_within_build = "";
    std::string cmake_build_type = "Debug";
    OverrideSettings override_settings{};
};

struct Config {
    std::string version = "";
    UnchangableSettings unchangable_settings{};
    Settings settings{};
    Directories directories{};
    std::unordered_map<std::string, FormatProgram> format = {};
    std::unordered_map<std::string, LintProgram> lint = {};
    std::unordered_map<std::string, BuildSystem> build_systems = {};
    std::unordered_map<std::string, Toolchain> toolchains = {};
    std::unordered_map<std::string, Command> commands = {};
    std::unordered_map<std::string, ProfileEntry> profiles = {};
};

const constexpr std::string_view config_schema = R"json(
{
  "$schema": "http://json-schema.org/draft-07/schema#",
  "type": "object",
  "required": [
    "version",
    "unchangable_settings",
    "settings",
    "directories",
    "format",
    "lint",
    "build_systems",
    "toolchains",
    "commands",
    "profiles"
  ],
  "properties": {
    "version": {
      "$ref": "#/definitions/string"
    },
    "unchangable_settings": {
      "$ref": "#/definitions/unchangable_settings"
    },
    "settings": {
      "$ref": "#/definitions/settings"
    },
    "directories": {
      "$ref": "#/definitions/directories"
    },
    "format": {
      "type": "object",
      "additionalProperties": {
        "$ref": "#/definitions/format_program"
      }
    },
    "lint": {
      "type": "object",
      "additionalProperties": {
        "$ref": "#/definitions/lint_program"
      }
    },
    "build_systems": {
      "type": "object",
      "additionalProperties": {
        "$ref": "#/definitions/build_system"
      }
    },
    "toolchains": {
      "type": "object",
      "additionalProperties": {
        "$ref": "#/definitions/toolchain"
      }
    },
    "commands": {
      "type": "object",
      "additionalProperties": {
        "$ref": "#/definitions/command"
      }
    },
    "profiles": {
      "type": "object",
      "additionalProperties": {
        "$ref": "#/definitions/profile_entry"
      }
    }
  },

  "definitions": {
    "string": {
      "type": ["string"]
    },

    "unchangable_settings": {
      "type": "object",
      "required": [
        "project",
        "language",
        "meta_build_system"
      ],
      "properties": {
        "project": {"$ref": "#/definitions/string"},
        "language": {"$ref": "#/definitions/string"},
        "meta_build_system": {"$ref": "#/definitions/string"}
      }
    },

    "cmake": {
      "type": "object",
      "required": [
        "path",
        "configure_arguments",
        "build_arguments"
      ],
      "properties": {
        "path": {"$ref": "#/definitions/string"},
        "configure_arguments": {
          "type": "array",
          "items": {"type": "string"}
        },
        "build_arguments": {
          "type": "array",
          "items": {"type": "string"}
        }
      }
    },

    "settings": {
      "type": "object",
      "required": [
        "binary",
        "build_system",
        "toolchain",
        "formatter",
        "linter",
        "default_profile",
        "cmake"
      ],
      "properties": {
        "binary": {"$ref": "#/definitions/string"},
        "build_system": {"$ref": "#/definitions/string"},
        "toolchain": {"$ref": "#/definitions/string"},
        "formatter": {"$ref": "#/definitions/string"},
        "linter": {"$ref": "#/definitions/string"},
        "default_profile": {"$ref": "#/definitions/string"},
        "cmake": {
          "$ref": "#/definitions/cmake"
        }
      }
    },

    "override_settings": {
      "type": "object",
      "properties": {
        "binary": {"$ref": "#/definitions/string"},
        "build_system": {"$ref": "#/definitions/string"},
        "toolchain": {"$ref": "#/definitions/string"},
        "formatter": {"$ref": "#/definitions/string"},
        "linter": {"$ref": "#/definitions/string"},
        "cmake": {
          "$ref": "#/definitions/cmake"
        }
      }
    },

    "directories": {
      "type": "object",
      "required": [
        "balk_path",
        "build_path",
        "build_benchmarks_path",
        "build_examples_path",
        "build_tests_path",
        "cmake_path",
        "examples_path",
        "export_path",
        "headers_path",
        "source_path",
        "tests_path",
        "benchmarks_path"
      ],
      "properties": {
        "balk_path": {"$ref": "#/definitions/string"},
        "build_path": {"$ref": "#/definitions/string"},
        "build_benchmarks_path": {"$ref": "#/definitions/string"},
        "build_examples_path": {"$ref": "#/definitions/string"},
        "build_tests_path": {"$ref": "#/definitions/string"},
        "cmake_path": {"$ref": "#/definitions/string"},
        "examples_path": {"$ref": "#/definitions/string"},
        "export_path": {"$ref": "#/definitions/string"},
        "headers_path": {"$ref": "#/definitions/string"},
        "source_path": {"$ref": "#/definitions/string"},
        "tests_path": {"$ref": "#/definitions/string"},
        "benchmarks_path": {"$ref": "#/definitions/string"}
      }
    },

    "format_program": {
      "type": "object",
      "required": [
        "binary",
        "preappended_arguments",
        "appended_arguments"
      ],
      "properties": {
        "binary": {"$ref": "#/definitions/string"},
        "preappended_arguments": {
          "type": "array",
          "items": {"type": "string"}
        },
        "appended_arguments": {
          "type": "array",
          "items": {"type": "string"}
        }
      }
    },

    "lint_program": {
      "type": "object",
      "required": [
        "binary",
        "preappended_arguments",
        "appended_arguments"
      ],
      "properties": {
        "binary": {"$ref": "#/definitions/string"},
        "preappended_arguments": {
          "type": "array",
          "items": {"type": "string"}
        },
        "appended_arguments": {
          "type": "array",
          "items": {"type": "string"}
        }
      }
    },

    "build_system": {
      "type": "object",
      "required": [
        "build_type",
        "path"
      ],
      "properties": {
        "build_type": {"$ref": "#/definitions/string"},
        "path": {"$ref": "#/definitions/string"}
      }
    },

    "toolchain": {
      "type": "object",
      "required": [
        "linker",
        "assembler",
        "c_compiler",
        "cpp_compiler"
      ],
      "properties": {
        "linker": {"$ref": "#/definitions/string"},
        "assembler": {"$ref": "#/definitions/string"},
        "c_compiler": {"$ref": "#/definitions/string"},
        "cpp_compiler": {"$ref": "#/definitions/string"}
      }
    },

    "command": {
      "type": "object",
      "required": [
        "binary",
        "preappended_arguments",
        "appended_arguments"
      ],
      "properties": {
        "binary": {"$ref": "#/definitions/string"},
        "preappended_arguments": {
          "type": "array",
          "items": {"type": "string"}
        },
        "appended_arguments": {
          "type": "array",
          "items": {"type": "string"}
        }
      }
    },

    "profile_entry": {
      "type": "object",
      "required": [
        "path_within_build",
        "cmake_build_type",
        "override_settings"
      ],
      "properties": {
        "path_within_build": {"$ref": "#/definitions/string"},
        "cmake_build_type": {"$ref": "#/definitions/string"},
        "override_settings": {
          "$ref": "#/definitions/override_settings"
        }
      }
    }
  }
}
)json";
}

namespace config {
inline void to_json(json& j, const UnchangableSettings& v) {
    j = json{
        {"project", v.project},
        {"language", v.language},
        {"meta_build_system", v.meta_build_system},
    };
}

inline void from_json(const json& j, UnchangableSettings& v) {
    j.at("project").get_to(v.project);
    j.at("language").get_to(v.language);
    j.at("meta_build_system").get_to(v.meta_build_system);
}

inline void to_json(json& j, const Cmake& v) {
    j = json{
        {"path", v.path},
        {"configure_arguments", v.configure_arguments},
        {"build_arguments", v.build_arguments},
    };
}

inline void from_json(const json& j, Cmake& v) {
    j.at("path").get_to(v.path);
    j.at("configure_arguments").get_to(v.configure_arguments);
    j.at("build_arguments").get_to(v.build_arguments);
}

inline void to_json(json& j, const Settings& v) {
    j = json{
        {"binary", v.binary},
        {"build_system", v.build_system},
        {"toolchain", v.toolchain},
        {"formatter", v.formatter},
        {"linter", v.linter},
        {"default_profile", v.default_profile},
        {"cmake", v.cmake},
    };
}

inline void from_json(const json& j, Settings& v) {
    j.at("binary").get_to(v.binary);
    j.at("build_system").get_to(v.build_system);
    j.at("toolchain").get_to(v.toolchain);
    j.at("formatter").get_to(v.formatter);
    j.at("linter").get_to(v.linter);
    j.at("default_profile").get_to(v.default_profile);
    j.at("cmake").get_to(v.cmake);
}

inline void to_json(json& j, const OverrideSettings& v) {
    j = json{
        {"binary", v.binary},
        {"build_system", v.build_system},
        {"toolchain", v.toolchain},
        {"formatter", v.formatter},
        {"linter", v.linter},
        {"cmake", v.cmake},
    };
}

inline void from_json(const json& j, OverrideSettings& v) {
    if(j.contains("binary")) {
        j.at("binary").get_to(v.binary);
    }

    if(j.contains("build_system")) {
        j.at("build_system").get_to(v.build_system);
    }

    if(j.contains("toolchain")) {
        j.at("toolchain").get_to(v.toolchain);
    }

    if(j.contains("formatter")) {
        j.at("formatter").get_to(v.formatter);
    }

    if(j.contains("linter")) {
        j.at("linter").get_to(v.linter);
    }

    if(j.contains("cmake")) {
        j.at("cmake").get_to(v.cmake);
    }
}

inline void to_json(json& j, const Directories& v) {
    j = json{
        {"balk_path", v.balk_path},
        {"build_path", v.build_path},
        {"build_benchmarks_path", v.build_benchmarks_path},
        {"build_examples_path", v.build_examples_path},
        {"build_tests_path", v.build_tests_path},
        {"cmake_path", v.cmake_path},
        {"examples_path", v.examples_path},
        {"export_path", v.export_path},
        {"headers_path", v.headers_path},
        {"source_path", v.source_path},
        {"tests_path", v.tests_path},
        {"benchmarks_path", v.benchmarks_path},
    };
}

inline void from_json(const json& j, Directories& v) {
    j.at("balk_path").get_to(v.balk_path);
    j.at("build_path").get_to(v.build_path);
    j.at("build_benchmarks_path").get_to(v.build_benchmarks_path);
    j.at("build_examples_path").get_to(v.build_examples_path);
    j.at("build_tests_path").get_to(v.build_tests_path);
    j.at("cmake_path").get_to(v.cmake_path);
    j.at("examples_path").get_to(v.examples_path);
    j.at("export_path").get_to(v.export_path);
    j.at("headers_path").get_to(v.headers_path);
    j.at("source_path").get_to(v.source_path);
    j.at("tests_path").get_to(v.tests_path);
    j.at("benchmarks_path").get_to(v.benchmarks_path);
}

inline void to_json(json& j, const FormatProgram& v) {
    j = json{
        {"binary", v.binary},
        {"preappended_arguments", v.preappended_arguments},
        {"appended_arguments", v.appended_arguments},
    };
}

inline void from_json(const json& j, FormatProgram& v) {
    j.at("binary").get_to(v.binary);
    j.at("preappended_arguments").get_to(v.preappended_arguments);
    j.at("appended_arguments").get_to(v.appended_arguments);
}

inline void to_json(json& j, const LintProgram& v) {
    j = json{
        {"binary", v.binary},
        {"preappended_arguments", v.preappended_arguments},
        {"appended_arguments", v.appended_arguments},
    };
}

inline void from_json(const json& j, LintProgram& v) {
    j.at("binary").get_to(v.binary);
    j.at("preappended_arguments").get_to(v.preappended_arguments);
    j.at("appended_arguments").get_to(v.appended_arguments);
}

inline void to_json(json& j, const BuildSystem& v) {
    j = json{
        {"build_type", v.build_type},
        {"path", v.path},
    };
}

inline void from_json(const json& j, BuildSystem& v) {
    j.at("build_type").get_to(v.build_type);
    j.at("path").get_to(v.path);
}

inline void to_json(json& j, const Toolchain& v) {
    j = json{
        {"linker", v.linker},
        {"assembler", v.assembler},
        {"c_compiler", v.c_compiler},
        {"cpp_compiler", v.cpp_compiler},
    };
}

inline void from_json(const json& j, Toolchain& v) {
    j.at("linker").get_to(v.linker);
    j.at("assembler").get_to(v.assembler);
    j.at("c_compiler").get_to(v.c_compiler);
    j.at("cpp_compiler").get_to(v.cpp_compiler);
}

inline void to_json(json& j, const Command& v) {
    j = json{
        {"binary", v.binary},
        {"preappended_arguments", v.preappended_arguments},
        {"appended_arguments", v.appended_arguments},
    };
}

inline void from_json(const json& j, Command& v) {
    j.at("binary").get_to(v.binary);
    j.at("preappended_arguments").get_to(v.preappended_arguments);
    j.at("appended_arguments").get_to(v.appended_arguments);
}

inline void to_json(json& j, const ProfileEntry& v) {
    j = json{
        {"path_within_build", v.path_within_build},
        {"cmake_build_type", v.cmake_build_type},
        {"override_settings", v.override_settings},
    };
}

inline void from_json(const json& j, ProfileEntry& v) {
    j.at("path_within_build").get_to(v.path_within_build);
    j.at("cmake_build_type").get_to(v.cmake_build_type);
    j.at("override_settings").get_to(v.override_settings);
}

inline void to_json(json& j, const Config& v) {
    j = json{
        {"version", v.version},
        {"unchangable_settings", v.unchangable_settings},
        {"settings", v.settings},
        {"directories", v.directories},
        {"format", v.format},
        {"lint", v.lint},
        {"build_systems", v.build_systems},
        {"toolchains", v.toolchains},
        {"commands", v.commands},
        {"profiles", v.profiles},
    };
}

inline void from_json(const json& j, Config& v) {
    j.at("version").get_to(v.version);
    j.at("unchangable_settings").get_to(v.unchangable_settings);
    j.at("settings").get_to(v.settings);
    j.at("directories").get_to(v.directories);
    j.at("format").get_to(v.format);
    j.at("lint").get_to(v.lint);
    j.at("build_systems").get_to(v.build_systems);
    j.at("toolchains").get_to(v.toolchains);
    j.at("commands").get_to(v.commands);
    j.at("profiles").get_to(v.profiles);
}
}
