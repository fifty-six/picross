//
// Created by home on 11/5/21.
//

#include <fstream>
#include <charconv>
#include <sstream>
#include <cassert>

#include <fmt/core.h>
#include <fmt/format.h>

#include <tl/expected.hpp>

#include "constraints.h"

namespace picross {

Constraints::Constraints(
        size_t size,
        vec<vec<int>> &&rows_,
        vec<vec<int>> &&cols_
)
        : size(size), rows(rows_), cols(cols_) {
    assert(rows.size() == static_cast<size_t>(size));
    assert(cols.size() == static_cast<size_t>(size));
}

tl::expected<Constraints, std::string> read_file(std::string const &name) {
    std::ifstream file;

    file.open(name);

    if (!file)
        return tl::unexpected(std::string("File not found!"));

    size_t size;

    file >> size;

    vec<vec<int>> rows;
    vec<vec<int>> cols;

    rows.reserve(size);
    cols.reserve(size);

    std::string line;
    size_t n = 0;

    // After the number it's just going to be \n I think
    std::getline(file, line, '\n');

    while (file.good()) {
        // Read n of size for rows, then cols.
        vec<vec<int>> &current = n >= size
                                 ? cols
                                 : rows;

        // Line
        std::getline(file, line, '\n');

        if (line.empty())
            continue;

        std::stringstream l_stream(line);

        vec<int> line_vec;
        std::string num;

        // Process line for our row/col constraints
        while (l_stream.good()) {
            std::getline(l_stream, num, ' ');

            size_t res;

            auto[ptr, ec] = std::from_chars(num.data(), num.data() + num.size(), res);

            if (ec != std::errc()) {
                return tl::unexpected(fmt::format("{} while parsing {}", std::make_error_code(ec).message(), num));
            }

            line_vec.push_back(static_cast<int>(res));
        }

        current.push_back(std::move(line_vec));

        ++n;
    }

    return Constraints { size, std::move(rows), std::move(cols) };
}

}