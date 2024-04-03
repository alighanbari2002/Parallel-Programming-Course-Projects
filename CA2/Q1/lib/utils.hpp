#ifndef UTILS_HPP
#define UTILS_HPP

#include <cstddef>
#include <string>
#include <chrono>
#include <cfloat>
#ifdef _WIN32
#include <intrin.h>
#else
#include <x86intrin.h>
#endif

namespace utils {

    namespace config {
        constexpr size_t ARRAY_SIZE = 1048576; // 2 ^ 20
    } // namespace config

    namespace performance {
        constexpr size_t UNROLL_FACTOR = 4; // The number of times to unroll the loop for optimization
    } // namespace performance

    namespace simd {
        constexpr size_t SIMD_VECTOR_LENGTH = 4; // The number of elements in a SIMD vector
        const __m128 MAX_FLOAT_VECTOR = _mm_set1_ps(FLT_MAX); // Set all elements to the maximum float value
    } // namespace simd

} // namespace utils

void print_group_info();

float* create_array();

void clean_up_array(float*& array);

std::chrono::high_resolution_clock::time_point get_current_time();

template <typename T>
long long calculate_duration(const T& start_time, const T& finish_time)
{
    return std::chrono::duration_cast<std::chrono::nanoseconds>(finish_time - start_time).count();
}

std::string format_time(const long long& time_ns);

void fill_array_with_random_values(float*& array);

#endif // UTILS_HPP
