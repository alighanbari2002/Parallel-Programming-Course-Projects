#ifndef FUNCTIONS_HPP
#define FUNCTIONS_HPP

#include <cstddef>

long long min_search_serial(const float* const &array);

void* find_local_min(void* arg);

long long min_search_parallel(const float* const &array, const size_t& num_threads);

#endif // FUNCTIONS_HPP
