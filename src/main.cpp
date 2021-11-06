#include <iostream>
#include <span>
#include <cstring>
#include <vector>
#include <cassert>
#include <fstream>
#include <charconv>
#include <sstream>
#include <optional>
#include <chrono>

#include "tl/expected.hpp"

#include <fmt/core.h>
#include <fmt/compile.h>
#include <fmt/color.h>
#include <fmt/chrono.h>
#include <fmt/ranges.h>

#include "main.h"
#include "solver.h"

using u32 = uint32_t;

Constraints::Constraints(
        size_t size,
        std::vector<std::vector<int>> &&rows_,
        std::vector<std::vector<int>> &&cols_
)
        : size(size), rows(rows_), cols(cols_) {
    assert(rows.size() == static_cast<size_t>(size));
    assert(cols.size() == static_cast<size_t>(size));
}

int main(int argc, char **argv) {
    std::span args { argv, static_cast<size_t>(argc) };

    if (args.size() > 2) {
        fmt::print(stderr, "Usage: {} FILE\n", args[0]);
        return 1;
    }

    std::string file_name = args.size() == 1
                            ? "default"
                            : args[1];

    auto constraints = read_file(file_name);

    if (!constraints) {
        fmt::print(stderr, fmt::emphasis::bold | fg(fmt::color::crimson), "{}\n", constraints.error());
        return 1;
    }

    fmt::print("Rows: {}\n", constraints.value().rows);
    fmt::print("Columns: {}\n", constraints.value().cols);
    
    // vec<u32> board = {
    //         0b001,
    //         0b11111,
    //         0b111111,
    //         0b111111,
    //         0b111100000001010,
    //         0b110000000111111,
    //         0b111000001011111,
    //         0b110000001111111,
    //         0b111001110111,
    //         0b1111111101,
    //         0b111111,
    //         0b111111110111,
    //         0b111111110111,
    //         0b110011111111
    // };
    
    // vec<u32> board =  {
    //         // 0b10011,
    //         // 0b11010,
    //         // 0b10100,
    //         // 0b11001,
    //         // 0b01111
    //         0b11001,
    //         0b01011,
    //         0b00101,
    //         0b10011,
    //         0b11110
    // };
    
    // fmt::print("{}\n", check_cols(*constraints, board, { 5, 1 }));
    
    auto start = std::chrono::steady_clock::now();
    
    auto board = solve(*constraints);
    
    auto elapsed = std::chrono::steady_clock::now() - start;
    fmt::print("Elapsed time: {:%S}s\n", elapsed);

    if (board) {
        fmt::print("Found solution!\n");
        
        auto fmt_string = fmt::format("{{:0{}b}}", board->size());
        
        for (auto row : *board) {
            std::string line = fmt::vformat(fmt_string, fmt::make_format_args(row));
            std::reverse(line.begin(), line.end());
            fmt::print("{}\n", line);
        }
        fmt::print("\n");
    } else {
        fmt::print(fg(fmt::color::crimson) | fmt::emphasis::bold, "No solution found. :(\n");
    }

    return 0;
}

expected<Constraints, std::string> read_file(std::string const &name) {
    std::ifstream file;

    file.open(name);

    if (!file)
        return unexpected(std::string("File not found!"));

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
            
            auto [ptr, ec] = std::from_chars(num.data(), num.data() + num.size(), res);
            
            if (ec != std::errc()) {
                return unexpected(fmt::format("{} while parsing {}", std::make_error_code(ec).message(), num));
            }
            
            line_vec.push_back(static_cast<int>(res));
        }

        current.push_back(std::move(line_vec));
        
        ++n;
    }
    
    fmt::print("{}, {}\n", rows, cols);

    return Constraints { size, std::move(rows), std::move(cols) };
}
