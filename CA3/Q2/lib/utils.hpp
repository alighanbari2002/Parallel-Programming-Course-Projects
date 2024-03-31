#ifndef UTILS_HPP
#define UTILS_HPP

#include <cstddef>

namespace utils {

    namespace config {
        constexpr size_t ARRAY_SIZE = 1048576; // 2 ^ 20
    } // namespace config

} // namespace utils

void print_group_info();

float* create_array();

void clean_up_array(float*& array);

double get_current_time();

long long calculate_duration(const double& start_time, const double& finish_time);

const char* format_time(const long long& time_ns);

void fill_array_with_random_values(float*& array);

#endif // UTILS_HPP
