#include <cstdio>
#include <cstddef>
#include <random>
#include <sstream>
#include <string>
#include <chrono>

#include "utils.hpp"

void print_group_info()
{
    printf("Group Members:\n");
    printf("\t- Ali Ghanbari [810199473]\n");
    printf("\t- Behrad Elmi  [810199557]\n");
}

float* create_array()
{
	return new float[utils::config::ARRAY_SIZE];
}

void clean_up_array(float*& array)
{
    delete[] array;
	array = nullptr;
}

std::chrono::high_resolution_clock::time_point get_current_time()
{
    return std::chrono::high_resolution_clock::now();
}

std::string format_time(const long long& time_ns)
{
    std::stringstream time_formatter;
    time_formatter.imbue(std::locale(""));
    time_formatter << time_ns;
    return time_formatter.str();
}

void fill_array_with_random_values(float*& array)
{
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<float> distr(0.0f, 1e7);

	for (size_t i = 0; i < utils::config::ARRAY_SIZE; ++i)
	{
		array[i] = distr(gen);
	}
}
