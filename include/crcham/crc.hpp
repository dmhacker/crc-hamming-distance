#ifndef CRCHAM_CRC_HPP
#define CRCHAM_CRC_HPP

#include <cstddef>
#include <cstdint>

namespace crcham {

class NaiveCRC {
private:
    size_t d_polylen;
    uint64_t d_generator;

public:
    __device__ __host__
    NaiveCRC(uint64_t koopman);

    __device__ __host__
    uint64_t normal() const;
    __device__ __host__
    size_t length() const;

    __device__ __host__
    uint64_t compute(const uint8_t* bytes, size_t bytelen) const;
};


class TabularCRC {
private:
    size_t d_polylen;
    uint64_t d_generator;
    uint64_t d_table[256];

public:
    __device__ __host__
    TabularCRC(uint64_t koopman);

    __device__ __host__
    uint64_t normal() const;
    __device__ __host__
    size_t length() const;

    __device__ __host__
    uint64_t compute(const uint8_t* bytes, size_t bytelen) const;
};

}

#endif
