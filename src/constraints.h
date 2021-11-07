#pragma once

#include <string>
#include <tl/expected.hpp>

#include "aliases.h"

namespace picross {

struct Constraints {
    size_t size;
    vec<vec<u32>> rows;
    vec<vec<u32>> cols;

    explicit Constraints(size_t size, vec<vec<u32>> &&rows, vec<vec<u32>> &&cols);
};

tl::expected<Constraints, std::string> read_file(std::string const &name);

}