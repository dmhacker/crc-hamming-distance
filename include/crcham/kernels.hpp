#ifndef CRCHAM_KERNELS_HPP
#define CRCHAM_KERNELS_HPP

#include <cassert>

#include <crcham/crc.hpp>
#include <crcham/permute.hpp>
#include <crcham/math.hpp>

namespace crcham {

template <class CRC>
__global__
void hammingWeightHeap(size_t* weights, CRC crc, size_t message_bits, size_t error_bits);

template <class CRC>
__global__
void hammingWeightHeap(size_t* weights, CRC crc, size_t message_bits, size_t error_bits) {
    size_t codeword_bits = message_bits + crc.length();
    size_t codeword_size = codeword_bits / 32;
    if (codeword_bits % 32 != 0) {
        codeword_size++;
    }
    auto codeword = static_cast<uint32_t*>(malloc(codeword_size * sizeof(uint32_t)));
    
    size_t pincr = gridDim.x * blockDim.x; 
    uint64_t pidx = blockIdx.x * blockDim.x + threadIdx.x; 
    uint64_t pmax = ncrll(codeword_bits, error_bits);

    size_t weight = 0;
    for (; pidx < pmax; pidx += pincr) {
        permute(codeword, codeword_size, pidx, codeword_bits, error_bits);
        assert(popcount(codeword, codeword_size) == error_bits); 
        weight++;
    }
    weights[blockIdx.x * blockDim.x + threadIdx.x] = 1;

    free(codeword);
}

}

#endif