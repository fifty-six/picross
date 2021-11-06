#include <iostream>
#include <span>
#include <cstring>
#include <vector>
#include <fstream>
#include <sstream>
#include <chrono>

#include <fmt/core.h>
#include <fmt/compile.h>
#include <fmt/color.h>
#include <fmt/chrono.h>
#include <fmt/ranges.h>

#include "aliases.h"
#include "solver.h"

int main(int argc, char **argv) {
    using namespace picross;
    
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
    auto end = std::chrono::steady_clock::now();
    
    fmt::print(
            "Elapsed time: {}\n",
            std::chrono::duration_cast<std::chrono::nanoseconds>(end - start)
    );

    if (board) {
        fmt::print("Found solution!\n");

        auto fmt_string = fmt::format("{{:0{}b}}", board->size());

        for (auto row: *board) {
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
