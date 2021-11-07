#include <cassert>
#include <optional>

#include "solver.h"

#include "fmt/format.h"
#include "fmt/ranges.h"

namespace picross {

auto solve(Constraints &c) -> std::optional<vec<u32>> {
    vec<u32> board(c.size);

    vec<u32> possibles(c.size);
    for (size_t i = 0; i < c.size; i++)
        possibles[i] = (1 << c.size) - 1;

    if (!solve(c, possibles, board, { 0, 0 }))
        return std::nullopt;

    return board;
}

void print_board(vec<u32> &board) {
    auto fmt_string = fmt::format("{{:0{}b}}", board.size());

    fmt::print("\n");
    for (auto row: board) {
        std::string line = fmt::vformat(fmt_string, fmt::make_format_args(row));
        std::reverse(line.begin(), line.end());
        fmt::print("{}\n", line);
    }
    fmt::print("\n");
}

auto inline get_column(vec<u32> &board, size_t ind) -> u32 {
    u32 col = 0;

    size_t n = 0;
    for (u32 row: board) {
        col |= (row & (1 << ind)) >> ind << n++;
    }

    return col;
}

auto check_cols_intermediate(Constraints &c, vec<u32> &board, pos_t current) {
    auto [row, cur_rc] = current;
    
    assert (board.size() == c.size);
    
    for (size_t i = 0; i < c.size; i++) {
        u32 col = get_column(board, i);
        
        u32 start = 0;
        for (auto constraint: c.cols[i]) {
            if (row < constraint)
                break;

            // fmt::print("col: {:010b}, i: {}\n", col, i);
            // fmt::print("c.col[i] = {}\n", c.cols[i]);
            
            u32 mask = (1 << constraint) - 1;
            
            // enum ConstraintSatisfied {
            //     Satisfied,
            //     
            // };

            auto check = [&]() -> i8 {
                // bool has_constraint = false;
                i8 has_constraint = 0;
                
                // fmt::print("start = {} for constraint {}\n", start, constraint);
                for (size_t j = start; j <= c.size - static_cast<size_t>(constraint); j++) {
                    u32 moved_mask = mask << j;

                    if ((moved_mask & col) != moved_mask) {
                        continue;
                    }
                    
                    has_constraint = -1;

                    // Put 1s on both sides of the mask 
                    u32 padding = (moved_mask << 1) | (moved_mask >> 1);
                    // Now remove the mask from the int
                    padding &= ~moved_mask;

                    // Now check that those 1s aren't set.
                    if ((~padding & col) == col) {
                        // fmt::print("{:010b} - c: {} completed.\n", col, constraint);
                        start = static_cast<u32>(j + constraint);
                        
                        return 1;
                    }
                    
                    // fmt::print("found but not padded correctly {:010b} - c: {}\n", col, constraint);
                }

                return has_constraint;
            };
            
            i8 checked = check();
            
            if (checked == -1) {
                // fmt::print("constraint {} for col {} failed\n", constraint, i);
                return false;
            } else if (checked == 0) {
                break;
            }
        }
    }
    
    return true;
}

auto check_cols(Constraints &c, vec<u32> &board, pos_t current) -> bool {
    auto[cur_row, cur_constraint] = current;

    assert(cur_row <= board.size());

    // for now
    if (cur_row != board.size()) {
        return check_cols_intermediate(c, board, current);
    }

    auto size = board.size();

    for (size_t i = 0; i < board.size(); i++) {
        u32 col = get_column(board, i);

        auto col_c = c.cols[i];

        for (auto constraint: col_c) {
            u32 mask = (1 << constraint) - 1;

            auto check = [&]() {
                for (size_t j = 0; j <= size - static_cast<size_t>(constraint); j++) {
                    u32 moved_mask = mask << j;

                    if ((moved_mask & col) != moved_mask) {
                        continue;
                    }

                    // Put 1s on both sides of the mask 
                    u32 padding = (moved_mask << 1) | (moved_mask >> 1);
                    // Now remove the mask from the int
                    padding &= ~moved_mask;

                    // Now check that those 1s aren't set.
                    if ((~padding & col) == col)
                        return true;
                }

                return false;
            };

            if (!check())
                return false;
        }
    }

    return true;
}

auto solve(Constraints &c, vec<u32> &possibles, vec<u32> &board, pos_t current) -> bool {
    auto[row, constraint_ind] = current;

    if (!check_cols(c, board, current))
        return false;

    if (row == board.size()) {
        fmt::print("{}\n", board);
        return true;
    }

    vec<u32> &row_reqs = c.rows[row];
    u32 orig_row = board[row];

    u32 &row_pos = possibles[row];
    u32 orig_pos = row_pos;

    // Can't *start* where others already are
    row_pos &= ~orig_row;

    assert(constraint_ind < row_reqs.size());

    auto constraint = row_reqs[constraint_ind];

    // fmt::print(
    //         "(r, c) = ({}, {}), c: {}, row: {:015b}, possibilities: {}\n",
    //         row, constraint_ind, constraint, orig_row, orig_pos
    // );

    u32 max = 1 << board.size();

    while (row_pos != 0) {
        // First possibility
        u32 bit = row_pos & (~row_pos + 1);

        // the block
        u32 block = (((bit << constraint) - 1) & ~(bit - 1)) | bit;

        // If the block we're trying to place at the 
        // first row possibility doesn't work, then
        // we lose, restore the original possibilities.
        if (block >= max) {
            row_pos = orig_pos;
            assert(board[row] == orig_row);
            return false;
        }

        pos_t next = { row, constraint_ind + 1 };

        // Roll over to next row once we're done with our constraints
        if (constraint_ind == row_reqs.size() - 1) {
            next = { row + 1, 0 };
        }

        auto pos = row_pos;

        // Shift it over 1 so they stay non-contiguous regions
        row_pos &= ~((block << 1) | block);
        board[row] |= block;

        if (solve(c, possibles, board, next)) {
            return true;
        }

        board[row] &= ~block;

        // Restore the possibilities and then
        // remove the bit instead of the entire block
        // as we only rule out the bit as the start.
        row_pos = pos;
        row_pos ^= bit;
    }

    // We have to restore this despite changing our
    // possibilities back on recursive call
    // because we could've gone forwards a row.
    row_pos = orig_pos;

    // This should be unnecessary, but safety is nice.
    assert(board[row] == orig_row);
    board[row] = orig_row;

    // Exhausted our possibilities.
    return false;
}

}