/*
** Copyright (c) 2020 Valve Corporation
** Copyright (c) 2020-2023 LunarG, Inc.
**
** Permission is hereby granted, free of charge, to any person obtaining a
** copy of this software and associated documentation files (the "Software"),
** to deal in the Software without restriction, including without limitation
** the rights to use, copy, modify, merge, publish, distribute, sublicense,
** and/or sell copies of the Software, and to permit persons to whom the
** Software is furnished to do so, subject to the following conditions:
**
** The above copyright notice and this permission notice shall be included in
** all copies or substantial portions of the Software.
**
** THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
** IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
** FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
** AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
** LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING
** FROM, OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER
** DEALINGS IN THE SOFTWARE.
*/

#pragma once

#include "util/defines.h"

#include <cstddef>
#include <functional>

GFXRECON_BEGIN_NAMESPACE(gfxrecon)
GFXRECON_BEGIN_NAMESPACE(util)
GFXRECON_BEGIN_NAMESPACE(hash)

template <class Type>
Type ClosestLowerPrime(void)
{
    // Next lower prime indexed using the number of bits in the size of
    // the return type to determine a prime number that can be used in the
    // hash function multiplication.
    // For example: the indices corresponding to the structure size are:
    //       [0] 0 bytes (not valid), [1] 1 byte, [2] 2 bytes, ...
    // NOTE: Some of these sizes won't be valid for standard architectures, but
    //       implementing them in the array just made the indexing process
    //       easier.
    uint8_t primes[]  = { 0, 7, 13, 23, 31, 37, 43, 53, 61 };
    size_t  type_size = sizeof(Type);
    assert(type_size < sizeof(primes));
    return static_cast<Type>(primes[type_size]);
}

template <class Type>
Type GenerateCheckSum(const uint8_t* code, size_t code_size)
{
    Type current_sum   = code_size;
    Type closest_prime = ClosestLowerPrime<Type>();
    for (Type i = 0; i < code_size; ++i)
    {
        current_sum = (current_sum * closest_prime) + std::hash<uint8_t>{}(code[i]);
    }
    return current_sum;
}

/**
 * @brief       hash_combine can be used to create a hash-value and combine with an existing hash-value.
 *
 * Called repeatedly to incrementally create a hash value from several variables.
 *
 * @tparam  T       value template-type
 * @param   seed    a provided reference to a seed. will be combined with the newly created value-hash.
 * @param   v       a provided value
 */
template <class T>
inline void hash_combine(std::size_t& seed, const T& v)
{
    std::hash<T> hasher;
    seed ^= hasher(v) + 0x9e3779b9 + (seed << 6U) + (seed >> 2U);
}

/**
 * @brief       hash_range can be used to create hash-values for arbitrary ranges.
 *
 * Requires an existing std::hash overload for the range's element-type.
 *
 * @tparam It   iterator template-type
 * @param first iterator to the beginning of a range
 * @param last  iterator to the end of the same range
 * @return      a newly created hash-value for the entire range.
 */
template <typename It>
std::size_t hash_range(It first, It last)
{
    std::size_t seed = 0;
    for (; first != last; ++first)
    {
        hash_combine(seed, *first);
    }
    return seed;
}

GFXRECON_END_NAMESPACE(hash)
GFXRECON_END_NAMESPACE(util)
GFXRECON_END_NAMESPACE(gfxrecon)
