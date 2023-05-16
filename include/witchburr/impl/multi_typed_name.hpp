// SPDX-FileCopyrightText: Copyright (c) 2022 merryhime <https://mary.rs>
// SPDX-License-Identifier: MIT

#pragma once

namespace witchburr {

template<auto... Vs>
struct MultiTypedName;

template<>
struct MultiTypedName<> {};

template<auto V, auto... Vs>
struct MultiTypedName<V, Vs...> : public MultiTypedName<Vs...> {
    constexpr operator decltype(V)() const { return V; }
};

}  // namespace witchburr
