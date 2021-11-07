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
        u32 row_count_,
        u32 col_count_,
        vec<vec<u32>> &&rows_,
        vec<vec<u32>> &&cols_
)
        : row_count(row_count_), col_count(col_count_), rows(rows_), cols(cols_) {
    assert(rows.size() == static_cast<size_t>(row_count_));
    assert(cols.size() == static_cast<size_t>(col_count_));
}

tl::expected<Constraints, std::string> read_file(std::string const &name) {
    std::ifstream file;

    file.open(name);

    if (!file)
        return tl::unexpected(std::string("File not found!"));

    u32 row_count, col_count;

    file >> row_count;
    file >> col_count;
    
    vec<vec<u32>> rows;
    vec<vec<u32>> cols;

    rows.reserve(row_count);
    cols.reserve(col_count);

    std::string line;
    size_t n = 0;

    // After the number it's just going to be \n I think
    std::getline(file, line, '\n');

    while (file.good()) {
        // Read n of size for rows, then cols.
        vec<vec<u32>> &current = n >= row_count
                                 ? cols
                                 : rows;

        // Line
        std::getline(file, line, '\n');

        if (line.empty())
            continue;

        std::stringstream l_stream(line);

        vec<u32> line_vec;
        std::string num;

        // Process line for our row/col constraints
        while (l_stream.good()) {
            std::getline(l_stream, num, ' ');

            u32 res;

            auto[ptr, ec] = std::from_chars(num.data(), num.data() + num.size(), res);

            if (ec != std::errc()) {
                return tl::unexpected(fmt::format("{} while parsing \"{}\"", std::make_error_code(ec).message(), num));
            }
            
            // 0 means empty, i.e. no constraint
            if (res != 0)
                line_vec.push_back(res);
        }

        current.push_back(std::move(line_vec));

        ++n;
    }

    return Constraints { row_count, col_count, std::move(rows), std::move(cols) };
}

}