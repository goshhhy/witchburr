// SPDX-FileCopyrightText: Copyright (c) 2022 merryhime <https://mary.rs>
// SPDX-FileCopyrightText: Copyright (c) 2023 linear cannon <dev@linear.network>
// SPDX-License-Identifier: MIT

#pragma once

#include "enum.hpp"
#include "witchburr/witchburr_exception.hpp"

namespace witchburr {

template<std::size_t bit_size_>
struct Imm {
public:
    static_assert(bit_size_ != 0 && bit_size_ <= 32, "Invalid bit_size");
    static constexpr std::size_t bit_size = bit_size_;
    static constexpr std::uint32_t mask = (1 << bit_size) - 1;

    constexpr /* implicit */ Imm(std::uint32_t value_)
        : m_value(value_)
    {
        if (!is_valid(value_))
            throw WitchburrException{ExceptionType::ImmOutOfRange};
    }

    constexpr auto operator<=>(const Imm& other) const { return m_value <=> other.m_value; }
    constexpr auto operator<=>(std::uint32_t other) const { return operator<=>(Imm{other}); }

    constexpr std::uint32_t value() const { return m_value; }

    static bool is_valid(std::uint32_t value_)
    {
        return ((value_ & mask) == value_);
    }

private:
    template<typename Policy>
    friend class BasicCodeGenerator;
    std::uint32_t m_value;
};

}
