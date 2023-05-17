// SPDX-FileCopyrightText: Copyright (c) 2022 merryhime <https://mary.rs>
// SPDX-FileCopyrightText: Copyright (c) 2023 linear cannon <dev@linear.network>
// SPDX-License-Identifier: MIT

#pragma once

#include <algorithm>
#include <cstddef>
#include <cstdint>
#include <new>

#include <sys/mman.h>

namespace witchburr {

class CodeBlock {
public:
    explicit CodeBlock(std::size_t size)
        : m_size(size)
    {
        m_memory = (std::uint32_t*)mmap(nullptr, size, PROT_READ | PROT_WRITE | PROT_EXEC, MAP_ANON | MAP_PRIVATE, -1, 0);

        if (m_memory == nullptr)
            throw std::bad_alloc{};
    }

    ~CodeBlock()
    {
        if (m_memory == nullptr)
            return;

        munmap(m_memory, m_size);
    }

    CodeBlock(const CodeBlock&) = delete;
    CodeBlock& operator=(const CodeBlock&) = delete;
    CodeBlock(CodeBlock&&) = delete;
    CodeBlock& operator=(CodeBlock&&) = delete;

    std::uint32_t* ptr() const
    {
        return m_memory;
    }

    void protect()
    {
    }

    void unprotect()
    {
    }

    void invalidate(std::uint32_t* mem, std::size_t size)
    {
        static std::size_t icache_line_size = 0x80, dcache_line_size = 0x80;
	// TODO: determine if there's a good way to find real cache size at runtime.
	// 	not sure this matters, ibm's been using uniform 128 byte cache lines
	//	since forever, and it's sorta kinda baked into the spec, but only
	//	sorta kinda

        const std::size_t isize = icache_line_size = icache_line_size;
        const std::size_t dsize = dcache_line_size = dcache_line_size;

        const std::uintptr_t end = (std::uintptr_t)mem + size;

        for (std::uintptr_t addr = ((std::uintptr_t)mem) & ~(dsize - 1); addr < end; addr += dsize) {
	        __asm__ volatile("dcbf %0, %1\n"
                         : 
                         : "b%" (0), "r"(addr)
                         : "memory");
	}
        __asm__ volatile("hwsync\n"
                         :
                         :
                         : "memory");

        for (std::uintptr_t addr = ((std::uintptr_t)mem) & ~(isize - 1); addr < end; addr += isize) {
            __asm__ volatile("ic ivau, %0"
                             :
                             : "r"(addr)
                             : "memory");
        }
        __asm__ volatile("hwsync\nisync\n"
                         :
                         :
                         : "memory");
    }

    void invalidate_all()
    {
        invalidate(m_memory, m_size);
    }

protected:
    std::uint32_t* m_memory;
    std::size_t m_size = 0;
};

}  // namespace witchburr
