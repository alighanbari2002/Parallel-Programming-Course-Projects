#include <cstdio>
#include <cstddef>
#include <string>
#include <cmath>
#ifdef _WIN32
#include <intrin.h>
#else
#include <x86intrin.h>
#endif

#include "functions.hpp"
#include "utils.hpp"

long long compute_avg_and_std_serial(const float* const &array)
{
	double sum_array[4] = {0.0}, sq_sum_array[4] = {0.0};
	double sum, sq_sum, average, standard_deviation;

	size_t i;

	// Start the timer
	auto start_time = get_current_time();

	for (i = 0; i < utils::config::ARRAY_SIZE; i += 4)
	{
		sum_array[0] += array[i];
		sq_sum_array[0] += array[i] * array[i];
	}

	for (i = 1; i < utils::config::ARRAY_SIZE; i += 4)
	{
		sum_array[1] += array[i];
		sq_sum_array[1] += array[i] * array[i];
	}

	for (i = 2; i < utils::config::ARRAY_SIZE; i += 4)
	{
		sum_array[2] += array[i];
		sq_sum_array[2] += array[i] * array[i];
	}

	for (i = 3; i < utils::config::ARRAY_SIZE; i += 4)
	{
		sum_array[3] += array[i];
		sq_sum_array[3] += array[i] * array[i];
	}

	sum = sum_array[0] + sum_array[1] + sum_array[2] + sum_array[3]; 
	sq_sum = sq_sum_array[0] + sq_sum_array[1] + sq_sum_array[2] + sq_sum_array[3];

	// Stop the timer
	auto finish_time = get_current_time();

	average = sum / utils::config::ARRAY_SIZE;
	standard_deviation = std::sqrt(sq_sum / utils::config::ARRAY_SIZE - average * average);

	long long elapsed_time = calculate_duration(start_time, finish_time);

	printf("\nSerial Method:\n");
	printf("\t- Average: %.4f\n", average);
	printf("\t- Standard Deviation: %.4f\n", standard_deviation);
	printf("\t- Run Time (ns): %s\n", format_time(elapsed_time).c_str());

	return elapsed_time;
}

inline void calculate_sums(const float* const &array, __m128d& vsum, __m128d& vsq_sum)
{
	__m128 v = _mm_load_ps(array);
	__m128 v_sq = _mm_mul_ps(v, v);

	__m128d vlow = _mm_cvtps_pd(v);
	__m128d vhigh = _mm_cvtps_pd(_mm_movehl_ps(v, v));
	vsum = _mm_add_pd(vsum, _mm_hadd_pd(vlow, vhigh));

	__m128d vlow_sq = _mm_cvtps_pd(v_sq);
	__m128d vhigh_sq = _mm_cvtps_pd(_mm_movehl_ps(v_sq, v_sq));
	vsq_sum = _mm_add_pd(vsq_sum, _mm_hadd_pd(vlow_sq, vhigh_sq));
}

long long compute_avg_and_std_parallel(const float* const &array)
{
	double sum_array[2], sq_sum_array[2];
	double sum = 0, sq_sum = 0, average, standard_deviation;

	__m128d vsum = _mm_setzero_pd(), vsq_sum = _mm_setzero_pd();

	size_t i;

	// Start the timer
	auto start_time = get_current_time();

	for (i = 0; i < utils::config::ARRAY_SIZE; i += utils::performance::UNROLL_FACTOR * utils::simd::SIMD_VECTOR_LENGTH)
	{
		// Load and process the first 4 elements
		calculate_sums(&array[i], vsum, vsq_sum);

		// Repeat the same process for the next 4 elements
		calculate_sums(&array[i + utils::simd::SIMD_VECTOR_LENGTH], vsum, vsq_sum);

		// Repeat the same process for the next 4 elements
		calculate_sums(&array[i + 2 * utils::simd::SIMD_VECTOR_LENGTH], vsum, vsq_sum);

		// Repeat the same process for the next 4 elements
		calculate_sums(&array[i + 3 * utils::simd::SIMD_VECTOR_LENGTH], vsum, vsq_sum);
	}

	_mm_store_pd(sum_array, vsum);
	_mm_store_pd(sq_sum_array, vsq_sum);

	sum += sum_array[0] + sum_array[1];
	sq_sum += sq_sum_array[0] + sq_sum_array[1];

	// Stop the timer
	auto finish_time = get_current_time();

	average = sum / utils::config::ARRAY_SIZE;
	standard_deviation = std::sqrt(sq_sum / utils::config::ARRAY_SIZE - average * average);

	long long elapsed_time = calculate_duration(start_time, finish_time);

	printf("\nParallel Method:\n");
	printf("\t- Average: %.4f\n", average);
	printf("\t- Standard Deviation: %.4f\n", standard_deviation);
	printf("\t- Run Time (ns): %s\n", format_time(elapsed_time).c_str());

	return elapsed_time;
}
