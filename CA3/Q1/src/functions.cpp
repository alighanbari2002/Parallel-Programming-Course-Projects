#include <cstdio>
#include <cstddef>
#include <string>
#include <cfloat>
#include <omp.h>

#include "functions.hpp"
#include "utils.hpp"

long long min_search_serial(const float* const &array)
{
	array_element_t min_element = {array[0], 0};

	size_t i;

	// Start the timer
	double start_time = get_current_time();

	for (i = 1; i < utils::config::ARRAY_SIZE; ++i)
	{
		if (array[i] < min_element.value)
		{
			min_element.value = array[i];
			min_element.index = i;
		}
	}

	// Stop the timer
	double finish_time = get_current_time();

	long long elapsed_time = calculate_duration(start_time, finish_time);

	printf("\nSerial Method:\n");
	printf("\t- Min Value: %f\n", min_element.value);
	printf("\t- Min Index: %d\n", min_element.index);
	printf("\t- Run Time (ns): %s\n", format_time(elapsed_time).c_str());

	return elapsed_time;
}

long long min_search_parallel(const float* const &array)
{
	array_element_t min_element = {array[0], 0};

	#pragma omp declare reduction(minimum : array_element_t : \
			omp_out = omp_in.value < omp_out.value ? omp_in : omp_out) \
			initializer(omp_priv = {FLT_MAX, -1})

	size_t i;

	// Start the timer
	double start_time = get_current_time();

	#pragma omp parallel for simd default(shared) private(i) \
			reduction(minimum:min_element) schedule(static)
		for (i = 1; i < utils::config::ARRAY_SIZE; ++i)
		{
			if (array[i] < min_element.value)
			{
				min_element.value = array[i];
				min_element.index = i;
			}
		}

	// Stop the timer
	double finish_time = get_current_time();

	long long elapsed_time = calculate_duration(start_time, finish_time);

	printf("\nParallel Method:\n");
	printf("\t- Min Value: %f\n", min_element.value);
	printf("\t- Min Index: %d\n", min_element.index);
	printf("\t- Run Time (ns): %s\n", format_time(elapsed_time).c_str());

	return elapsed_time;
}
