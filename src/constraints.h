#pragma once

#include <string>
#include <tl/expected.hpp>

#include "aliases.h"

namespace picross {

struct Constraints {
    size_t size;
    vec<vec<int>> rows;
    vec<vec<int>> cols;

    explicit Constraints(size_t size, std::vector<std::vector<int>> &&rows, std::vector<std::vector<int>> &&cols);
};

tl::expected<Constraints, std::string> read_file(std::string const &name);

}