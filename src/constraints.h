#pragma once

#include <string>
#include <system_error>
#include <variant>
#include <tl/expected.hpp>

#include "misc.h"

namespace picross {

struct Constraints {
    u32 row_count;
    u32 col_count;
    vec<vec<u32>> rows;
    vec<vec<u32>> cols;

    explicit Constraints(u32 row_count, u32 col_count, vec<vec<u32>> &&rows, vec<vec<u32>> &&cols);
};

struct ParseError {
    std::error_code code;
    std::string line;

    explicit ParseError(std::errc errc, std::string&& line) : code(std::make_error_code(errc)), line(std::move(line)) {}
    explicit ParseError(std::errc errc, std::string_view line) : ParseError(errc, std::string(line)) {}

    auto message() -> std::string;
};

struct ReadError : std::variant<std::errc, ParseError> {
    // constructors
    using std::variant<std::errc, ParseError>::variant;

    auto message() -> std::string;
};

tl::expected<Constraints, ReadError> read_file(std::string const &name);

}