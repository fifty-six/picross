#include <iostream>
#include <span>
#include <cstring>
#include <fstream>
#include <sstream>
#include <chrono>
#include <cassert>

#include <fmt/core.h>
#include <fmt/compile.h>
#include <fmt/color.h>
#include <fmt/chrono.h>
#include <fmt/ranges.h>

#include "aliases.h"
#include "solver.h"

void test_cols();

int main(int argc, char **argv) {
    using namespace picross;

    std::span args { argv, static_cast<size_t>(argc) };

    if (args.size() > 2) {
        fmt::print(stderr, "Usage: {} [--test] | FILE\n", args[0]);
        return 1;
    }

    if (args.size() == 2 && std::string_view(args[1]) == "--test") {
        test_cols();
        return 0;
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
    fmt::print("Columns: {}\n\n", constraints.value().cols);

    auto start = std::chrono::steady_clock::now();
    auto board = solve(*constraints);
    auto end = std::chrono::steady_clock::now();

    if (board) {
        fmt::print(fg(fmt::color::light_green) | fmt::emphasis::bold, "Found solution!\n");
        print_board(*constraints, *board);
    } else {
        fmt::print(fg(fmt::color::crimson) | fmt::emphasis::bold, "No solution found. :(\n");
    }
    
    fmt::print("Elapsed time: ");
    
    fmt::print(
            fg(fmt::color::light_blue) | fmt::emphasis::bold,
            "{}\n",
            std::chrono::duration_cast<std::chrono::milliseconds>(end - start)
    );

    return 0;
}

void test_cols() {
    {
        picross::Constraints constraints {
                5, 5,
                {{ 2, 2 }, { 3 }, { 1, 1 }, { 1, 1 }, { 1, 1 }},
                {{ 4 }, { 2, 1 }, { 2 }, { 1 }, { 1, 2 }}
        };

        // Note the board looks backwards because
        // we reverse the output on each row.
        vec<u32> board = {
                0b11011,
                0b00111,
                0b00101,
                0b10001,
                0b10010
        };

        assert(check_cols(constraints, board, { 5, 1 }));
    }

    {
        picross::Constraints constraints {
                10, 10,
                {{ 2, 2 }, { 1, 1, 1 }, { 1, 4, 1 }, { 1, 1, 4 }, { 2, 2, 1 }, { 1, 2, 2, 1 }, { 3, 2 }, { 1, 3, 2 }, { 2, 1, 2 }, { 3, 1, 1, 1 }},
                {{ 3, 3 }, { 3, 2 }, { 1, 2, 1 }, { 2, 3 }, { 1, 1, 1, 1, 1 }, { 1, 1, 1 }, { 5, 2 }, { 1, 3 }, { 1, 1, 3 }, { 6, 2 }}
        };

        vec<u32> valid_board = {
                0b1100110000,
                0b1001000001,
                0b1001111001,
                0b1111001001,
                0b1001100110,
                0b1011011010,
                0b0110001110,
                0b0110011101,
                0b1101000011,
                0b1001010111
        };

        assert(check_cols(constraints, valid_board, { 10, 0 }));
    }
    {
        picross::Constraints constraints {
                10, 10,
                {{ 2, 3, 1 }, { 2, 1, 1 }, { 2, 2, 3 }, { 3, 3 }, { 1, 5, 1 }, { 2, 6 }, { 1, 3, 1 }, { 1, 1, 1, 1 }, { 1, 1, 2, 1 }, { 2, 1, 1, 1 }},
                {{ 2, 2, 3 }, { 4, 2, 1 }, { 1, 1, 1 }, { 1, 4, 1 }, { 1, 4 }, { 1, 1, 3, 1 }, { 1, 3, 2 }, { 6, 1 }, { 2, 2, 1 }, { 1, 1, 1, 2 }}
        };

        vec<u32> invalid_board = {
                187, 646, 475, 910, 761, 507, 370, 149, 357, 587
        };

        assert(!check_cols(constraints, invalid_board, { 10, 0 }));
        
        auto solved = picross::solve(constraints);
        
        if (!solved) {
            assert(false);
            return;
        }
        
        vec<u32> sol = { 742, 139, 947, 462, 761, 507, 314, 593, 709, 299 };
        
        assert(std::equal(solved->begin(), solved->end(), sol.begin(), sol.end()));
    }
}