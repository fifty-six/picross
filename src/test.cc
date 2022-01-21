#include "test.h"

#include "misc.h"
#include "solver.h"

#include <fmt/core.h>
#include <fmt/color.h>

namespace picross {

#ifdef __PRETTY_FUNCTION__
#define release_func __PRETTY_FUNCTION__
#else
#define release_func __func__
#endif

void test() {
    test_cols();

    auto success = fmt::format(
            fg(fmt::color::light_green) | fmt::emphasis::bold,
            "Tests passed!"
    );

    fmt::print("{}\n", success);
}

void test_cols() {
#define release_assert(expr) (static_cast<bool>(expr) ? static_cast<void>(0) : fail(#expr, __FILE__, __LINE__, release_func))

    auto fail = [](const std::string &expr, const std::string &file, int line, const std::string &func) {
        auto err = fmt::format(fg(fmt::color::indian_red) | fmt::emphasis::bold, "error: ");
        fmt::print(stderr, "{}", err);
        fmt::print(stderr, "{}:{}: {}: Assertion `{}` failed.\n", file, line, func, expr);
        std::terminate();
    };

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

        release_assert(check_cols(constraints, board, { 5, 1 }));
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

        release_assert(check_cols(constraints, valid_board, { 10, 0 }));
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

        release_assert(!check_cols(constraints, invalid_board, { 10, 0 }));

        auto solved = picross::solve(constraints);

        if (!solved) {
            release_assert(false);
            return;
        }

        vec<u32> sol = { 742, 139, 947, 462, 761, 507, 314, 593, 709, 299 };

        release_assert(std::equal(solved->begin(), solved->end(), sol.begin(), sol.end()));
    }
}

}