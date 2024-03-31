#include <cstdio>
#include <cstddef>
#ifdef _WIN32
#include <intrin.h>
#else
#include <x86intrin.h>
#endif

#include "functions.hpp"
#include "utils.hpp"

long long min_search_serial(const float* const &array)
{
	float min_elements[4];
	int min_indexes[4];

	float min_element;
	int min_index;

	size_t i;

	// Start the timer
	auto start_time = get_current_time();

	min_elements[0] = array[0];
	min_indexes[0] = 0;

	for (i = 0; i < utils::config::ARRAY_SIZE; i += 4)
	{
		if (array[i] < min_elements[0])
		{
			min_elements[0] = array[i];
			min_indexes[0] = i;
		}
	}

	min_elements[1] = array[1];
	min_indexes[1] = 1;

	for (i = 1; i < utils::config::ARRAY_SIZE; i += 4)
	{
		if (array[i] < min_elements[1])
		{
			min_elements[1] = array[i];
			min_indexes[1] = i;
		}
	}

	min_elements[2] = array[2];
	min_indexes[2] = 2;

	for (i = 2; i < utils::config::ARRAY_SIZE; i += 4)
	{
		if (array[i] < min_elements[2])
		{
			min_elements[2] = array[i];
			min_indexes[2] = i;
		}
	}

	min_elements[3] = array[3];
	min_indexes[3] = 3;

	for (i = 3; i < utils::config::ARRAY_SIZE; i += 4)
	{
		if (array[i] < min_elements[3])
		{
			min_elements[3] = array[i];
			min_indexes[3] = i;
		}
	}

	min_element = min_elements[0];
	min_index = min_indexes[0];

	for (i = 1; i < 4; ++i)
	{
		if (min_elements[i] < min_element)
		{
			min_element = min_elements[i];
			min_index = min_indexes[i];
		}
	}

	// Stop the timer
	auto finish_time = get_current_time();

	long long elapsed_time = calculate_duration(start_time, finish_time);

	printf("\nSerial Method:\n");
	printf("\t- Min Value: %f\n", min_element);
	printf("\t- Min Index: %d\n", min_index);
	printf("\t- Run Time (ns): %s\n", format_time(elapsed_time));

	return elapsed_time;
}

inline void update_min_elements(const float* const &array, __m128& min_elements, __m128& min_indices, __m128& indices)
{
	__m128 new_values = _mm_loadu_ps(array);
	__m128 comparison_mask = _mm_cmpgt_ps(min_elements, new_values);
	min_elements = _mm_blendv_ps(min_elements, new_values, comparison_mask);
	min_indices = _mm_blendv_ps(min_indices, indices, comparison_mask);
	indices = _mm_add_ps(indices, _mm_set1_ps(4)); // Increment indices by the vector size
}

long long min_search_parallel(const float* const &array)
{
	__m128 min_elements = utils::simd::MAX_FLOAT_VECTOR;
	__m128 indices = _mm_setr_ps(0, 1, 2, 3);
	__m128 min_indices = indices;

	float array_values[utils::simd::SIMD_VECTOR_LENGTH], array_indexes[utils::simd::SIMD_VECTOR_LENGTH];
	float min_element;
	int min_index;

	size_t i;

	// Start the timer
	auto start_time = get_current_time();

	for (i = 0; i < utils::config::ARRAY_SIZE; i += utils::performance::UNROLL_FACTOR * utils::simd::SIMD_VECTOR_LENGTH)
	{
		// Load and process the first 4 elements
		update_min_elements(&array[i], min_elements, min_indices, indices);

		// Repeat the same process for the next 4 elements
		update_min_elements(&array[i + utils::simd::SIMD_VECTOR_LENGTH], min_elements, min_indices, indices);

		// Repeat the same process for the next 4 elements
		update_min_elements(&array[i + 2 * utils::simd::SIMD_VECTOR_LENGTH], min_elements, min_indices, indices);

		// Repeat the same process for the next 4 elements
		update_min_elements(&array[i + 3 * utils::simd::SIMD_VECTOR_LENGTH], min_elements, min_indices, indices);
	}

	_mm_store_ps(array_values, min_elements);
	_mm_store_ps(array_indexes, min_indices);

	min_element = array_values[0];
	min_index = static_cast<int>(array_indexes[0]);

	for (i = 1; i < utils::simd::SIMD_VECTOR_LENGTH; ++i)
	{
		if (array_values[i] < min_element)
		{
			min_element = array_values[i];
			min_index = static_cast<int>(array_indexes[i]);
		}
	}

	// Stop the timer
	auto finish_time = get_current_time();

	long long elapsed_time = calculate_duration(start_time, finish_time);

	printf("\nParallel Method:\n");
	printf("\t- Min Value: %f\n", min_element);
	printf("\t- Min Index: %d\n", min_index);
	printf("\t- Run Time (ns): %s\n", format_time(elapsed_time));

	return elapsed_time;
}
