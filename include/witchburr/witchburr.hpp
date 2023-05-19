// SPDX-FileCopyrightText: Copyright (c) 2022 merryhime <https://mary.rs>
// SPDX-FileCopyrightText: Copyright (c) 2023 linear cannon <dev@linear.network>
// SPDX-License-Identifier: MIT

#pragma once

#include <bit>
#include <cstddef>
#include <cstdint>
#include <optional>
#include <tuple>
#include <type_traits>
#include <variant>
#include <vector>

#include "witchburr/impl/enum.hpp"
#include "witchburr/impl/imm.hpp"
#include "witchburr/impl/list.hpp"
#include "witchburr/impl/multi_typed_name.hpp"
#include "witchburr/impl/offset.hpp"
#include "witchburr/impl/reg.hpp"
#include "witchburr/impl/string_literal.hpp"
#include "witchburr/witchburr_exception.hpp"

namespace witchburr {

namespace detail {

template<StringLiteral bs, StringLiteral barg>
constexpr std::uint32_t get_bits()
{
    std::uint32_t result = 0;
    for (std::size_t i = 0; i < 32; i++) {
        for (std::size_t a = 0; a < barg.strlen; a++) {
            if (bs.value[i] == barg.value[a]) {
                result |= 1 << (31 - i);
            }
        }
    }
    return result;
}

template<class... Ts>
struct overloaded : Ts... {
    using Ts::operator()...;
};

template<class... Ts>
overloaded(Ts...) -> overloaded<Ts...>;

}  // namespace detail

struct Label {
public:
    Label() = default;

private:
    template<typename Policy>
    friend class BasicCodeGenerator;

    explicit Label(std::uintptr_t addr)
        : m_addr(addr)
    {}

    using EmitFunctionType = std::uint32_t (*)(std::uintptr_t wb_addr, std::uintptr_t resolved_addr);

    struct Writeback {
        std::uintptr_t m_wb_addr;
        std::uint32_t m_mask;
        EmitFunctionType m_fn;
    };

    std::optional<std::uintptr_t> m_addr;
    std::vector<Writeback> m_wbs;
};

template<typename Policy>
class BasicCodeGenerator : public Policy {
public:
    BasicCodeGenerator(typename Policy::constructor_argument_type arg)
        : Policy(arg)
    {}

    Label l()
    {
        return Label{Policy::current_address()};
    }

    void l(Label& label)
    {
        if (label.m_addr)
            throw WitchburrException{ExceptionType::LabelRedefinition};

        const auto target_addr = Policy::current_address();
        label.m_addr = target_addr;
        for (auto& wb : label.m_wbs) {
            const std::uint32_t value = wb.m_fn(wb.m_wb_addr, target_addr);
            Policy::set_at_address(wb.m_wb_addr, value, wb.m_mask);
        }
        label.m_wbs.clear();
    }

#include "witchburr/impl/mnemonics_branch_v2.03.inc.hpp"
#include "witchburr/impl/mnemonics_integer_v2.03.inc.hpp"

    void align(std::size_t alignment)
    {
        if (alignment < 4 || (alignment & (alignment - 1)) != 0)
            throw WitchburrException{ExceptionType::InvalidAlignment};

        while (Policy::template ptr<std::uintptr_t>() & (alignment - 1)) {
            NOP();
        }
    }

    void NOP()
    {
	ORI(GpReg{0}, GpReg{0}, 0);
    }

    void LI(GpReg rt, Imm<16> si)
    {
	ADDI(rt, GpReg{0}, si);
    }

    void dw(std::uint32_t value)
    {
        Policy::append(value);
    }

    void dx(std::uint64_t value)
    {
        Policy::append(static_cast<std::uint32_t>(value));
        Policy::append(static_cast<std::uint32_t>(value >> 32));
    }

private:
#include "witchburr/impl/ppc64_encode_helpers.inc.hpp"

    template<StringLiteral bs, StringLiteral... bargs, typename... Ts>
    void emit(Ts... args)
    {
        std::uint32_t encoding = detail::get_bits<bs, "1">();
        encoding |= (0 | ... | encode<detail::get_bits<bs, bargs>()>(std::forward<Ts>(args)));
        Policy::append(encoding);
    }

    template<std::uint32_t splat, std::size_t size, std::size_t align>
    std::uint32_t encode(AddrOffset<size, align> v)
    {
        static_assert(std::popcount(splat) == size - align);

        const auto encode_fn = [](std::uintptr_t current_addr, std::uintptr_t target) {
            const std::ptrdiff_t diff = target - current_addr;
            return pdep<splat>(AddrOffset<size, align>::encode(diff));
        };

        return std::visit(detail::overloaded{
                              [&](std::uint32_t encoding) {
                                  return pdep<splat>(encoding);
                              },
                              [&](Label* label) {
                                  if (label->m_addr) {
                                      return encode_fn(Policy::current_address(), *label->m_addr);
                                  }

                                  label->m_wbs.emplace_back(Label::Writeback{Policy::current_address(), ~splat, static_cast<Label::EmitFunctionType>(encode_fn)});
                                  return 0u;
                              },
                              [&](void* p) {
                                  return encode_fn(Policy::current_address(), reinterpret_cast<std::uintptr_t>(p));
                              },
                          },
                          v.m_payload);
    }

    template<std::uint32_t splat, std::size_t size, std::size_t shift_amount>
    std::uint32_t encode(PageOffset<size, shift_amount> v)
    {
        static_assert(std::popcount(splat) == size);


        const auto encode_fn = [](std::uintptr_t current_addr, std::uintptr_t target) {
            return pdep<splat>(PageOffset<size, shift_amount>::encode(current_addr, target));
        };

        return std::visit(detail::overloaded{
                              [&](Label* label) {
                                  if (label->m_addr) {
                                      return encode_fn(Policy::current_address(), *label->m_addr);
                                  }

                                  label->m_wbs.emplace_back(Label::Writeback{Policy::current_address(), ~splat, static_cast<Label::EmitFunctionType>(encode_fn)});
                                  return 0u;
                              },
                              [&](const void* p) {
                                  return encode_fn(Policy::current_address(), reinterpret_cast<std::uintptr_t>(p));
                              },
                          },
                          v.m_payload);
    }
};

struct PointerCodeGeneratorPolicy {
public:
    template<typename T>
    T ptr()
    {
        static_assert(std::is_pointer_v<T> || std::is_same_v<T, std::uintptr_t> || std::is_same_v<T, std::intptr_t>);
        return reinterpret_cast<T>(m_ptr);
    }

    void set_ptr(std::uint32_t* ptr_)
    {
        m_ptr = ptr_;
    }

protected:
    using constructor_argument_type = std::uint32_t*;

    PointerCodeGeneratorPolicy(std::uint32_t* ptr_)
        : m_ptr(ptr_)
    {}

    void append(std::uint32_t instruction)
    {
        *m_ptr++ = instruction;
    }

    std::uintptr_t current_address()
    {
        return reinterpret_cast<std::uintptr_t>(m_ptr);
    }

    void set_at_address(std::uintptr_t addr, std::uint32_t value, std::uint32_t mask)
    {
        std::uint32_t* p = reinterpret_cast<std::uint32_t*>(addr);
        *p = (*p & mask) | value;
    }

private:
    std::uint32_t* m_ptr;
};

using CodeGenerator = BasicCodeGenerator<PointerCodeGeneratorPolicy>;

namespace util {

inline constexpr GpReg r0{0}, r1{1}, r2{2}, r3{3}, r4{4}, r5{5}, r6{6}, r7{7}, r8{8}, r9{9}, r10{10}, r11{11}, r12{12}, r13{13}, r14{14}, r15{15}, r16{16}, r17{17}, r18{18}, r19{19}, r20{20}, r21{21}, r22{22}, r23{23}, r24{24}, r25{25}, r26{26}, r27{27}, r28{28}, r29{29}, r30{30}, r31{31};


}  // namespace util

}  // namespace witchburr 
