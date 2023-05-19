// SPDX-FileCopyrightText: Copyright (c) 2023 linear cannon <dev@linear.network>

#pragma once

namespace witchburr {
struct Reg;

struct GpReg;

struct Reg {
    constexpr explicit Reg(bool is_vector_, unsigned bitsize_, int index_)
        : m_index(index_)
        , m_bitsize(bitsize_)
        , m_is_vector(is_vector_)
    {
        assert(index_ >= -1 && index_ <= 31);
        assert(bitsize_ != 0 && (bitsize_ & (bitsize_ - 1)) == 0 && "Bitsize must be a power of two");
    }

    constexpr int index() const { return m_index; }
    constexpr unsigned bitsize() const { return m_bitsize; }
    constexpr bool is_vector() const { return m_is_vector; }

private:
    int m_index : 8;
    unsigned m_bitsize : 8;
    bool m_is_vector;
};

struct GpReg : public Reg {
    constexpr explicit GpReg(int index_)
        : Reg(false, 64, index_) {}

    template<typename Policy>
    friend class BasicCodeGenerator;
};

}
