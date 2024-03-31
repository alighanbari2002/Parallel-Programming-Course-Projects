#ifndef FUNCTIONS_HPP
#define FUNCTIONS_HPP

#ifdef _WIN32
#include <intrin.h>
#else
#include <x86intrin.h>
#endif

long long min_search_serial(const float* const &array);

inline void update_min_elements(const float* const &array, __m128& min_elements, __m128& min_indices, __m128& indices);

long long min_search_parallel(const float* const &array);

#endif // FUNCTIONS_HPP
