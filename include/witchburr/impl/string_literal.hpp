// SPDX-FileCopyrightText: Copyright (c) 2022 merryhime <https://mary.rs>
// SPDX-FileCopyrightText: Copyright (c) 2023 linear cannon <dev@linear.network>
// SPDX-License-Identifier: MIT

#pragma once

#include <algorithm>
#include <cstddef>

namespace witchburr {

template<size_t N>
struct StringLiteral {
    constexpr StringLiteral(const char (&str)[N])
    {
        std::copy_n(str, N, value);
    }

    static constexpr std::size_t strlen = N - 1;
    static constexpr std::size_t size = N;

    char value[N];
};

}  // namespace witchburr
