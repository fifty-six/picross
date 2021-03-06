#include <cassert>
#include <optional>

#include "solver.h"

#include "fmt/format.h"

namespace picross {

auto solve(Constraints &c) -> std::optional<vec<u32>> {
    vec<u32> board(c.row_count);

    vec<u32> possibles(c.row_count);
    for (size_t i = 0; i < c.row_count; i++)
        possibles[i] = (1 << c.col_count) - 1;

    if (!solve(c, possibles, board, { 0, 0 }))
        return std::nullopt;

    return board;
}

void print_board(Constraints &c, vec<u32> &board) {
    auto fmt_string = fmt::format("{{:0{}b}}", c.col_count);

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

auto check_cols(Constraints &c, vec<u32> &board, pos_t current) -> bool {
    auto[row, cur_rc] = current;

    assert (row <= c.row_count);

    bool is_end = c.row_count == row;

    for (size_t i = 0; i < c.col_count; i++) {
        u32 col = get_column(board, i);

        u32 start = 0;
        u32 sum = 0;
        for (auto constraint: c.cols[i]) {
            if (constraint == 0)
                break;
            
            sum += constraint;

            // No point in checking constraints that would go over the row requirement anyways
            if (row < sum)
                break;

            // Account for the blank after a constraint.
            sum++;

            u32 mask = (1 << constraint) - 1;

            enum class ConstraintSatisfied {
                Satisfied,
                Invalid,
                Missing
            };

            auto check = [&]() -> ConstraintSatisfied {
                auto has_constraint = ConstraintSatisfied::Missing;

                for (size_t j = start; j <= c.row_count - static_cast<size_t>(constraint); j++) {
                    u32 moved_mask = mask << j;

                    if ((moved_mask & col) != moved_mask) {
                        continue;
                    }

                    has_constraint = ConstraintSatisfied::Invalid;

                    // Put 1s on both sides of the mask 
                    u32 padding = (moved_mask << 1) | (moved_mask >> 1);
                    // Now remove the mask from the int
                    padding &= ~moved_mask;

                    // Now check that those 1s aren't set.
                    if ((~padding & col) == col) {
                        // Bump up the start for the next constraint,
                        // so that we don't double count.
                        start = static_cast<u32>(j + constraint);
                        return ConstraintSatisfied::Satisfied;
                    }
                }

                return has_constraint;
            };

            auto checked = check();

            if (checked == ConstraintSatisfied::Invalid || (checked == ConstraintSatisfied::Missing && is_end)) {
                return false;
            } else if (checked == ConstraintSatisfied::Missing) {
                break;
            }
        }
    }

    return true;
}

auto solve(Constraints &c, vec<u32> &possibles, vec<u32> &board, pos_t current) -> bool {
    auto[row, constraint_ind] = current;

    if (!check_cols(c, board, current))
        return false;

    if (row == c.row_count) {
        return true;
    }

    vec<u32> &row_reqs = c.rows[row];
    u32 orig_row = board[row];

    u32 &row_pos = possibles[row];
    u32 orig_pos = row_pos;

    // Can't *start* where others already are
    row_pos &= ~orig_row;

    assert(constraint_ind < row_reqs.size() || row_reqs.empty());
    
    if (row_reqs.empty()) {
        return solve(c, possibles, board, { row + 1, 0 });
    }

    auto constraint = row_reqs[constraint_ind];

    u32 max = 1 << c.col_count;

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