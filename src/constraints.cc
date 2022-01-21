//
// Created by home on 11/5/21.
//

#include <fstream>
#include <charconv>
#include <sstream>
#include <cassert>
#include <variant>

#include <fmt/core.h>
#include <fmt/compile.h>
#include <fmt/ranges.h>

#include <tl/expected.hpp>
#include <tl/getlines.hpp>
#include <tl/enumerate.hpp>

#include <utility>

#include "constraints.h"

namespace picross {

Constraints::Constraints(
        u32 row_count_,
        u32 col_count_,
        vec<vec<u32>> &&rows_,
        vec<vec<u32>> &&cols_
)
        : row_count(row_count_), col_count(col_count_), rows(std::move(rows_)), cols(std::move(cols_)) {
    assert(rows.size() == static_cast<size_t>(row_count_));
    assert(cols.size() == static_cast<size_t>(col_count_));
}

auto ParseError::message() -> std::string {
    return fmt::format("err: {{{}}} while parsing {}", code.message(), line);
}

auto ReadError::message() -> std::string {
    return std::visit(overloaded{
            [](std::errc err) {
                return std::move(std::make_error_code(err).message());
            },
            [](ParseError p_err) {
                return p_err.message();
            }
    }, *this);
}

tl::expected<Constraints, ReadError> read_file(std::string const &name) {
    std::ifstream file;

    file.open(name);

    if (!file) {
        return tl::unexpected(std::errc::no_such_file_or_directory);
    }

    auto parse_u32 = [](const std::string_view num) -> tl::expected<std::pair<u32, std::string_view>, ParseError> {
        u32 out;

        auto[ptr, ec] = std::from_chars(num.data(), num.data() + num.size(), out);

        if (ec != std::errc()) {
            return tl::unexpected(ParseError(ec, num));
        }

        return std::make_pair(out, std::string_view(ptr, &num.back() - ptr + 1));
    };

    u32 row_count, col_count;

    std::string counts;
    std::getline(file, counts, '\n');

    auto parse = parse_u32(counts)
            .and_then([&](auto &&pair) {
                auto[val, view] = pair;

                row_count = val;

                // skip the space between them
                return parse_u32(view.substr(1));
            })
            .and_then([&](auto &&pair) -> tl::expected<void, ParseError> {
                auto[val, _] = pair;

                col_count = val;

                return {};
            });

    if (!parse) {
        return tl::make_unexpected(parse.error());
    }

    vec<vec<u32>> rows;
    vec<vec<u32>> cols;

    rows.reserve(row_count);
    cols.reserve(col_count);

    for (auto&&[ind, line]: tl::views::getlines(file) | tl::views::enumerate) {
        // First read the rows, then the columns
        vec<vec<u32>> &current = ind < row_count ? rows : cols;

        std::istringstream l_stream(line);

        vec<u32> line_vec;

        // Process line for our row/col constraints
        for (std::string &num: tl::getlines_view(l_stream, ' ')) {
            auto res = parse_u32(num);

            if (!res) {
                return tl::unexpected(res.error());
            }

            auto[val, view] = *res;

            // 0 means empty, i.e. no constraint
            if (val != 0) {
                line_vec.push_back(val);
            }
        }

        current.push_back(std::move(line_vec));
    }

    return Constraints{row_count, col_count, std::move(rows), std::move(cols)};
}

}