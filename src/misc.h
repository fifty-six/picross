#pragma once

#include <vector>
#include <tuple>
#include <cstdint>

template<typename T>
using vec = std::vector<T>;

using pos_t = std::tuple<size_t, size_t>;

template<class... Ts> struct overloaded : Ts... { using Ts::operator()...; };

using u8  [[maybe_unused]] = uint8_t;
using u16 [[maybe_unused]] = uint16_t;
using u32 [[maybe_unused]] = uint32_t;
using u64 [[maybe_unused]] = uint64_t;

using i8  [[maybe_unused]] = int8_t;
using i16 [[maybe_unused]] = int16_t;
using i32 [[maybe_unused]] = int32_t;
using i64 [[maybe_unused]] = int64_t;