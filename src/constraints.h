#pragma once

#include <string>
#include <tl/expected.hpp>

#include "aliases.h"

namespace picross {

struct Constraints {
    u32 row_count;
    u32 col_count;
    vec<vec<u32>> rows;
    vec<vec<u32>> cols;

    explicit Constraints(u32 row_count, u32 col_count, vec<vec<u32>> &&rows, vec<vec<u32>> &&cols);
};

tl::expected<Constraints, std::string> read_file(std::string const &name);

}