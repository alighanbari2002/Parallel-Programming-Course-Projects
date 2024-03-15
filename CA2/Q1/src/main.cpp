#include <iostream>
#include <sstream>
#include <random>
#include <chrono>
#include <stdlib.h>
#include <stdio.h>
#include <float.h>
#include <time.h>
#include <math.h>
#ifdef 		_WIN32
#include <intrin.h>
#else
#include <x86intrin.h>
#endif

using std::default_random_engine; 
using std::uniform_real_distribution;
using std::chrono::high_resolution_clock;
using std::chrono::duration_cast;
using std::chrono::nanoseconds;
using std::stringstream;
using std::locale;

typedef long long ll;

#define ARRAY_SIZE 1048576 // 2 ^ 20

void generate_random_array(float*& array, const size_t& size)
{
	default_random_engine generator(time(NULL));
	uniform_real_distribution<float> distribution(0.0, pow(10, 6));

	for (size_t i = 0; i < size; ++i)
	{
		array[i] = distribution(generator);
	}
}

ll min_search_serial(float*& array, const size_t& size)
{
	float min_elements[4], min_element;
	int min_indexes[4], min_index;
	size_t i;

	// Start the timer
	auto start_time = high_resolution_clock::now();

	min_elements[0] = array[0];
	min_indexes[0] = 0;

	for (i = 0; i < size; i += 4)
	{
		if (array[i] < min_elements[0])
		{
			min_elements[0] = array[i];
			min_indexes[0] = i;
		}
	}

	min_elements[1] = array[1];
	min_indexes[1] = 1;

	for (i = 1; i < size; i += 4)
	{
		if (array[i] < min_elements[1])
		{
			min_elements[1] = array[i];
			min_indexes[1] = i;
		}
	}

	min_elements[2] = array[2];
	min_indexes[2] = 2;

	for (i = 2; i < size; i += 4)
	{
		if (array[i] < min_elements[2])
		{
			min_elements[2] = array[i];
			min_indexes[2] = i;
		}
	}

	min_elements[3] = array[3];
	min_indexes[3] = 3;

	for (i = 3; i < size; i += 4)
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
	auto finish_time = high_resolution_clock::now();

	ll execution_time = duration_cast<nanoseconds>(finish_time - start_time).count();

	// Use a string stream to format the output
	stringstream output_formatter;
	output_formatter.imbue(locale(""));
	output_formatter << execution_time;

	printf("\nSerial Method:\n");
	printf("\t- Min Value: %f\n", min_element);
	printf("\t- Min Index: %d\n", min_index);
	printf("\t- Run Time (ns): %s\n", output_formatter.str().c_str());

	return execution_time;
}

ll min_search_parallel(float*& array, const size_t& size)
{
	size_t i;

	const int UNROLL_FACTOR = 4; // The number of times to unroll the loop
	const int VECTOR_SIZE   = 4; // The size of the SIMD vector
	const __m128 INCREMENT  = _mm_set1_ps(VECTOR_SIZE);
	const __m128 MAX_VALUE  = _mm_set1_ps(FLT_MAX);

	__m128 min_elements = MAX_VALUE;
	__m128 indexes = _mm_setr_ps(0, 1, 2, 3);
	__m128 min_indexes = indexes;
	__m128 value, lt;

	float array_values[VECTOR_SIZE], array_indexes[VECTOR_SIZE];
	float min_element;
	int min_index;

	// Start the timer
	auto start_time = high_resolution_clock::now();

	for (i = 0; i < size; i += UNROLL_FACTOR * VECTOR_SIZE)
	{
		// Load the values from the array and compare them with the minimum elements
		value = _mm_loadu_ps(&array[i]);
		lt = _mm_cmpgt_ps(min_elements, value);
		min_elements = _mm_blendv_ps(min_elements, value, lt);
		min_indexes = _mm_blendv_ps(min_indexes, indexes, lt);
		indexes = _mm_add_ps(indexes, INCREMENT);

		// Repeat the same process for the next values
		value = _mm_loadu_ps(&array[i + VECTOR_SIZE]);
		lt = _mm_cmpgt_ps(min_elements, value);
		min_elements = _mm_blendv_ps(min_elements, value, lt);
		min_indexes = _mm_blendv_ps(min_indexes, indexes, lt);
		indexes = _mm_add_ps(indexes, INCREMENT);

		// Repeat the same process for the next values
		value = _mm_loadu_ps(&array[i + 2 * VECTOR_SIZE]);
		lt = _mm_cmpgt_ps(min_elements, value);
		min_elements = _mm_blendv_ps(min_elements, value, lt);
		min_indexes = _mm_blendv_ps(min_indexes, indexes, lt);
		indexes = _mm_add_ps(indexes, INCREMENT);

		// Repeat the same process for the next values
		value = _mm_loadu_ps(&array[i + 3 * VECTOR_SIZE]);
		lt = _mm_cmpgt_ps(min_elements, value);
		min_elements = _mm_blendv_ps(min_elements, value, lt);
		min_indexes = _mm_blendv_ps(min_indexes, indexes, lt);
		indexes = _mm_add_ps(indexes, INCREMENT);
	}

	_mm_storeu_ps(array_values, min_elements);
	_mm_storeu_ps(array_indexes, min_indexes);

	min_element = array_values[0];
	min_index = (int) array_indexes[0];

	for (i = 1; i < VECTOR_SIZE; ++i)
	{
		if (array_values[i] < min_element)
		{
			min_element = array_values[i];
			min_index = (int) array_indexes[i];
		}
	}

	// Stop the timer
	auto finish_time = high_resolution_clock::now();

	ll execution_time = duration_cast<nanoseconds>(finish_time - start_time).count();

	// Use a string stream to format the output
	stringstream output_formatter;
	output_formatter.imbue(locale(""));
	output_formatter << execution_time;

	printf("\nParallel Method:\n");
	printf("\t- Min Value: %f\n", min_element);
	printf("\t- Min Index: %d\n", min_index);
	printf("\t- Run Time (ns): %s\n", output_formatter.str().c_str());

	return execution_time;
}

void print_group_info()
{
	printf("Group Members:\n");
	printf("\t- Ali Ghanbari [810199473]\n");
	printf("\t- Behrad Elmi  [810199557]\n");
}

int main()
{
	print_group_info();

	float *array = new float [ARRAY_SIZE];
	generate_random_array(array, ARRAY_SIZE);

	ll serial_time = min_search_serial(array, ARRAY_SIZE);
	ll parallel_time = min_search_parallel(array, ARRAY_SIZE);

	delete array;

	printf("\nSpeedup: %.4lf\n", (double) serial_time / (double) parallel_time);

	return 0;
}
