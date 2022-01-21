#include <span>
#include <chrono>

#include <fmt/core.h>
#include <fmt/compile.h>
#include <fmt/color.h>
#include <fmt/chrono.h>
#include <fmt/ranges.h>

#include "solver.h"
#include "test.h"

int main(int argc, char **argv) {
    using namespace picross;

    std::span args { argv, static_cast<size_t>(argc) };

    if (args.size() > 2) {
        fmt::print(stderr, "Usage: {} [--test] | FILE\n", args[0]);
        return 1;
    }

    if (args.size() == 2 && std::string_view(args[1]) == "--test") {
        test();
        return 0;
    }

    std::string file_name = args.size() == 1
                            ? "default"
                            : args[1];

    auto constraints = read_file(file_name);

    if (!constraints) {
        fmt::print(stderr, fmt::emphasis::bold | fg(fmt::color::crimson), "{}\n", constraints.error().message());
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