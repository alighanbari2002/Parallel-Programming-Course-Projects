#include <cstdio>
#include <cstddef>
#include <string>
#include <cmath>
#include <omp.h>

#include "functions.hpp"
#include "utils.hpp"

long long compute_avg_and_std_serial(const float* const &array)
{
	double sum = 0, sq_sum = 0, average, standard_deviation;

	size_t i;

	// Start the timer
	double start_time = get_current_time();

	for (i = 0; i < utils::config::ARRAY_SIZE; ++i)
	{
		sum += array[i];
		sq_sum += array[i] * array[i];
	}

	// Stop the timer
	double finish_time = get_current_time();

	average = sum / utils::config::ARRAY_SIZE;
	standard_deviation = std::sqrt(sq_sum / utils::config::ARRAY_SIZE - average * average);

	long long elapsed_time = calculate_duration(start_time, finish_time);

	printf("\nSerial Method:\n");
	printf("\t- Average: %.4lf\n", average);
	printf("\t- Standard Deviation: %.4lf\n", standard_deviation);
	printf("\t- Run Time (ns): %s\n", format_time(elapsed_time).c_str());

	return elapsed_time;
}

long long compute_avg_and_std_parallel(const float* const &array)
{
	double sum = 0, sq_sum = 0, average, standard_deviation;

	size_t i;

	// Start the timer
	double start_time = get_current_time();

	#pragma omp parallel for simd default(shared) private(i) \
			reduction(+:sum,sq_sum) schedule(static)
		for (i = 0; i < utils::config::ARRAY_SIZE; ++i)
		{
			sum += array[i];
			sq_sum += array[i] * array[i];
		}

	// Stop the timer
	double finish_time = get_current_time();

	average = sum / utils::config::ARRAY_SIZE;
	standard_deviation = std::sqrt(sq_sum / utils::config::ARRAY_SIZE - average * average);

	long long elapsed_time = calculate_duration(start_time, finish_time);

	printf("\nParallel Method:\n");
	printf("\t- Average: %.4lf\n", average);
	printf("\t- Standard Deviation: %.4lf\n", standard_deviation);
	printf("\t- Run Time (ns): %s\n", format_time(elapsed_time).c_str());

	return elapsed_time;
}