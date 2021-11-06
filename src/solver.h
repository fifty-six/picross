#pragma once

#include <optional>
#include "aliases.h"
#include "constraints.h"

namespace picross {

void print_board(vec<u32> &board);
    
auto check_cols(Constraints &c, vec<u32> &board, pos_t current) -> bool;

auto solve(Constraints &c) -> std::optional<vec<u32>>;

auto solve(Constraints &c, vec<u32> &possibles, vec<u32> &board, pos_t current) -> bool;

}