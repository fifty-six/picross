//
// Created by home on 11/5/21.
//

#ifndef PICROSS_SOLVER_H
#define PICROSS_SOLVER_H

#include <optional>

#include "main.h"

using pos_t = std::tuple<size_t, size_t>;

auto check_cols(Constraints &c, vec<u32> &board, pos_t current) -> bool;
auto solve(Constraints &c) -> std::optional<vec<u32>>;
auto solve(Constraints &c, vec<u32> &possibles, vec<u32> &board, pos_t current) -> bool;

#endif //PICROSS_SOLVER_H
