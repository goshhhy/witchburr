// SPDX-FileCopyrightText: Copyright (c) 2023 merryhime <https://mary.rs>
// SPDX-License-Identifier: MIT

#pragma once

#include <exception>

namespace witchburr {

enum class ExceptionType {
#define WITCHBURR_EXCEPTION(tag, str) tag,
#include "witchburr/impl/witchburr_exception.inc.hpp"
#undef WITCHBURR_EXCEPTION
};

inline const char* to_string(ExceptionType et)
{
    switch (et) {
#define WITCHBURR_EXCEPTION(tag, str) \
    case ExceptionType::tag:       \
        return str;
#include "witchburr/impl/witchburr_exception.inc.hpp"
#undef WITCHBURR_EXCEPTION
    default:
        return "unknown ExceptionType";
    }
}

class WitchburrException : public std::exception {
public:
    explicit WitchburrException(ExceptionType et)
        : type{et}
    {}

    const char* what() const noexcept override
    {
        return to_string(type);
    }

private:
    ExceptionType type;
};

}  // namespace witchburr 
