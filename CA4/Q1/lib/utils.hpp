#ifndef UTILS_HPP
#define UTILS_HPP

#include <cstddef>
#include <string>
#include <chrono>

typedef struct {
    const float* array;
    size_t start;
    size_t end;
    float min_element;
    int min_index;
} thread_data_t;

namespace utils {

    namespace config {
        constexpr size_t ARRAY_SIZE = 1048576; // 2 ^ 20
    } // namespace config

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
