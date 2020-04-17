#ifndef CRCHAM_KERNELS_HPP
#define CRCHAM_KERNELS_HPP

#include <cassert>
#include <cstdio>

#include <crcham/codeword.hpp>
#include <crcham/crc.hpp>
#include <crcham/math.hpp>

namespace crcham {

template <class CRC>
__global__
void hammingWeight(size_t* weights, CRC crc, size_t message_bits, size_t error_bits) {
    // Allocate the minimum number of integers required to hold the message and FCS field
    size_t codeword_bits = message_bits + crc.length();
    size_t codeword_ints = codeword_bits / 32;
    if (codeword_bits % 32 != 0) {
        codeword_ints++;
    }
    auto codeword_int_ptr = static_cast<uint32_t*>(
        malloc(codeword_ints * sizeof(uint32_t)));

    // Get the little-endian byte represenentation of the int array
    auto codeword_byte_ptr = reinterpret_cast<uint8_t*>(
        codeword_int_ptr);
    size_t codeword_bytes = codeword_ints * 4;

    const size_t widx = blockIdx.x * blockDim.x + threadIdx.x;
    size_t pincr = gridDim.x * blockDim.x; 
    uint64_t pidx = blockIdx.x * blockDim.x + threadIdx.x; 
    uint64_t pmax = ncrll(codeword_bits, error_bits);
    size_t weight = 0;

    for (; pidx < pmax; pidx += pincr) {
        // Permute the bytes in the ${pidx}th way
        permute(codeword_int_ptr, codeword_ints, pidx, codeword_bits, error_bits);
        assert(popcount(codeword_int_ptr, codeword_ints) == error_bits); 
        // Fix little endianness issues
        for (size_t i = 0; i < codeword_bytes; i += 4) {
            uint8_t tmp;
            tmp = codeword_byte_ptr[i];
            codeword_byte_ptr[i] = codeword_byte_ptr[i + 3];
            codeword_byte_ptr[i + 3] = tmp;
            tmp = codeword_byte_ptr[i + 1];
            codeword_byte_ptr[i + 1] = codeword_byte_ptr[i + 2];
            codeword_byte_ptr[i + 2] = tmp;
        }
        // Test to see if the codeword with errors is considered valid
        uint64_t error_crc = extract(codeword_byte_ptr, codeword_bytes, codeword_bits, crc.length());
        uint64_t good_crc = crc.compute(codeword_byte_ptr, codeword_bytes);
        if (error_crc == good_crc) {
            weight++;
        }
    }
    weights[widx] = weight;

    free(codeword_int_ptr);
}

}

#endif
