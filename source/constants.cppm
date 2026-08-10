module;

#include <cstdint>

export module Constants;

export namespace constants {
    constexpr std::uint64_t version_major() {
        return 0;
    }

    constexpr std::uint64_t version_minor() {
        return 0;
    }

    constexpr std::uint64_t version_patch() {
        return 1;
    }
}
