// SPDX-FileCopyrightText: Copyright (c) 2022 merryhime <https://mary.rs>
// SPDX-FileCopyrightText: CopyRight (c) 2023 linear cannon <dev@linear.network>
// SPDX-License-Identifier: MIT

#include <cstdint>
#include <cstdio>
#include <limits>

#define CATCH_CONFIG_MAIN
#include <catch2/catch_test_macros.hpp>

#include "witchburr/code_block.hpp"
#include "witchburr/witchburr.hpp"
#include "rand_int.hpp"

using namespace witchburr;
using namespace witchburr::util;

TEST_CASE("Basic Test")
{
    CodeBlock mem{4096};
    CodeGenerator code{mem.ptr()};

    mem.unprotect();

    code.LI(r3, 42);
    code.BLR();

    mem.protect();
    mem.invalidate_all();

    int result = ((int (*)())mem.ptr())();
    REQUIRE(result == 42);
}
