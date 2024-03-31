#ifndef FUNCTIONS_HPP
#define FUNCTIONS_HPP

#ifdef _WIN32
#include <intrin.h>
#else
#include <x86intrin.h>
#endif

long long compute_avg_and_std_serial(const float* const &array);

inline void calculate_sums(const float* const &array, __m128d& vsum, __m128d& vsq_sum);

long long compute_avg_and_std_parallel(const float* const &array);

#endif // FUNCTIONS_HPP
