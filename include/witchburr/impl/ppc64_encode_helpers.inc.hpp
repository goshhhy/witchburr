// SPDX-FileCopyrightText: Copyright (c) 2022 merryhime <https://mary.rs>
// SPDX-FileCopyrightText: Copyright (c) 2023 linear cannon <dev@linear.network>
// SPDX-License-Identifier: MIT

#include "enum.hpp"
template<std::uint32_t mask_>
static constexpr std::uint32_t pdep(std::uint32_t val)
{
    std::uint32_t mask = mask_;
    std::uint32_t res = 0;
    for (std::uint32_t bb = 1; mask; bb += bb) {
        if (val & bb)
            res |= mask & -mask;
        mask &= mask - 1;
    }
    return res;
}

#define WITCHBURR_STD_ENCODE(TYPE, ACCESS, SIZE)                   \
    template<std::uint32_t splat>                               \
    std::uint32_t encode(TYPE v)                                \
    {                                                           \
        static_assert(std::popcount(splat) == SIZE);            \
        return pdep<splat>(static_cast<std::uint32_t>(ACCESS)); \
    }

WITCHBURR_STD_ENCODE(GpReg, v.index() & 31, 5)
WITCHBURR_STD_ENCODE(BOCond, v, 5)
WITCHBURR_STD_ENCODE(BranchHint, v, 2)
WITCHBURR_STD_ENCODE(LinkBit, v, 1)

template<std::uint32_t splat, std::size_t imm_size>
std::uint32_t encode(Imm<imm_size> v)
{
    static_assert(std::popcount(splat) >= imm_size);
    return pdep<splat>(v.value());
}
