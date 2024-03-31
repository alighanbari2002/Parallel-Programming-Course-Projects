#ifndef FUNCTIONS_HPP
#define FUNCTIONS_HPP

#include <cstddef>

long long compute_avg_and_std_serial(const float* const &array);

void* calculate_sums(void* arg);

long long compute_avg_and_std_parallel(const float* const &array, const size_t& num_threads);

#endif // FUNCTIONS_HPP
