#ifndef FUNCTIONS_HPP
#define FUNCTIONS_HPP

#ifdef _WIN32
#include <intrin.h>
#else
#include <x86intrin.h>
#endif

long long min_search_serial(const float* const &array);

long long min_search_parallel(const float* const &array);

#endif // FUNCTIONS_HPP
