//
// Created by home on 10/15/21.
//

#ifndef PICROSS_MAIN_H
#define PICROSS_MAIN_H

#include <string>
#include <vector>
#include "tl/expected.hpp"

template<typename T>
using vec = std::vector<T>;

using u32 = uint32_t;

using tl::expected;
using tl::unexpected;

struct Constraints {
    size_t size;
    vec<vec<int>> rows;
    vec<vec<int>> cols;

    explicit Constraints(size_t size, std::vector<std::vector<int>> &&rows, std::vector<std::vector<int>> &&cols);
};

tl::expected<Constraints, std::string> read_file(std::string const& name);

#endif //PICROSS_MAIN_H
