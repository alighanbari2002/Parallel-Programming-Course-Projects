#include <iostream>
#include <random>
#include <sstream>
#include <cstdio>
#include <cstdlib>
#include <cfloat>
#include <cmath>
#include <chrono>
#include <stdexcept>
#ifdef 		_WIN32
#include <intrin.h>
#else
#include <x86intrin.h>
#endif

void print_group_info()
{
    printf("Group Members:\n");
	printf("\t- Ali Ghanbari [810199473]\n");
	printf("\t- Behrad Elmi  [810199557]\n");
}

float* create_array(const size_t& array_size)
{
	float* array = static_cast<float*>(_mm_malloc(array_size * sizeof(float), 16));
	if (!array)
	{
		throw std::bad_alloc();
	}
	return array;
}

void clean_up_array(float*& array)
{
	_mm_free(array);
	array = nullptr;
}

std::chrono::high_resolution_clock::time_point get_current_time()
{
	return std::chrono::high_resolution_clock::now();
}

template <typename T>
long long calculate_duration(const T& start_time, const T& finish_time)
{
    return std::chrono::duration_cast<std::chrono::nanoseconds>(finish_time - start_time).count();
}

const char* format_time(const long long& time_ns)
{
    std::stringstream time_formatter;
    time_formatter.imbue(std::locale(""));
    time_formatter << time_ns;
    return time_formatter.str().c_str();
}

void fill_array_with_random_values(float*& array, const size_t& array_size)
{
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<float> distr(0.0f, 1e7);

	for (size_t i = 0; i < array_size; ++i)
	{
		array[i] = distr(gen);
	}
}

long long min_search_serial(const float* const &array, const size_t& array_size)
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

	for (i = 0; i < array_size; i += 4)
	{
		if (array[i] < min_elements[0])
		{
			min_elements[0] = array[i];
			min_indexes[0] = i;
		}
	}

	min_elements[1] = array[1];
	min_indexes[1] = 1;

	for (i = 1; i < array_size; i += 4)
	{
		if (array[i] < min_elements[1])
		{
			min_elements[1] = array[i];
			min_indexes[1] = i;
		}
	}

	min_elements[2] = array[2];
	min_indexes[2] = 2;

	for (i = 2; i < array_size; i += 4)
	{
		if (array[i] < min_elements[2])
		{
			min_elements[2] = array[i];
			min_indexes[2] = i;
		}
	}

	min_elements[3] = array[3];
	min_indexes[3] = 3;

	for (i = 3; i < array_size; i += 4)
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

long long min_search_parallel(const float* const &array, const size_t& array_size)
{
	const int UNROLL_FACTOR = 4; // The number of times to unroll the loop
	const int VECTOR_SIZE   = 4; // The size of the SIMD vector
	const __m128 INCREMENT  = _mm_set1_ps(VECTOR_SIZE);
	const __m128 MAX_VALUE  = _mm_set1_ps(FLT_MAX);

	__m128 min_elements = MAX_VALUE;
	__m128 indices = _mm_setr_ps(0, 1, 2, 3);
	__m128 min_indices = indices;

	__m128 new_values, comparison_mask;

	float array_values[VECTOR_SIZE], array_indexes[VECTOR_SIZE];
	float min_element;
	int min_index;

	size_t i;

	// Start the timer
	auto start_time = get_current_time();

	for (i = 0; i < array_size; i += UNROLL_FACTOR * VECTOR_SIZE)
	{
		// Load and process the first 4 elements
		new_values = _mm_load_ps(&array[i]);
		comparison_mask = _mm_cmpgt_ps(min_elements, new_values);
		min_elements = _mm_blendv_ps(min_elements, new_values, comparison_mask);
		min_indices = _mm_blendv_ps(min_indices, indices, comparison_mask);
		indices = _mm_add_ps(indices, INCREMENT);

		// Repeat the same process for the next 4 elements
		new_values = _mm_load_ps(&array[i + VECTOR_SIZE]);
		comparison_mask = _mm_cmpgt_ps(min_elements, new_values);
		min_elements = _mm_blendv_ps(min_elements, new_values, comparison_mask);
		min_indices = _mm_blendv_ps(min_indices, indices, comparison_mask);
		indices = _mm_add_ps(indices, INCREMENT);

		// Repeat the same process for the next 4 elements
		new_values = _mm_load_ps(&array[i + 2 * VECTOR_SIZE]);
		comparison_mask = _mm_cmpgt_ps(min_elements, new_values);
		min_elements = _mm_blendv_ps(min_elements, new_values, comparison_mask);
		min_indices = _mm_blendv_ps(min_indices, indices, comparison_mask);
		indices = _mm_add_ps(indices, INCREMENT);

		// Repeat the same process for the next 4 elements
		new_values = _mm_load_ps(&array[i + 3 * VECTOR_SIZE]);
		comparison_mask = _mm_cmpgt_ps(min_elements, new_values);
		min_elements = _mm_blendv_ps(min_elements, new_values, comparison_mask);
		min_indices = _mm_blendv_ps(min_indices, indices, comparison_mask);
		indices = _mm_add_ps(indices, INCREMENT);
	}

	_mm_store_ps(array_values, min_elements);
	_mm_store_ps(array_indexes, min_indices);

	min_element = array_values[0];
	min_index = static_cast<int>(array_indexes[0]);

	for (i = 1; i < VECTOR_SIZE; ++i)
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

int main()
{
	print_group_info();

	const size_t ARRAY_SIZE = 1048576; // 2 ^ 20
	float* array = create_array(ARRAY_SIZE);
	fill_array_with_random_values(array, ARRAY_SIZE);

	long long elapsed_time_serial = min_search_serial(array, ARRAY_SIZE);
	long long elapsed_time_parallel = min_search_parallel(array, ARRAY_SIZE);

	clean_up_array(array);

	printf("\nSpeedup: %.4lf\n", static_cast<double>(elapsed_time_serial) / elapsed_time_parallel);

	return EXIT_SUCCESS;
}
