#include <iostream>
#include <sstream>
#include <random>
#include <chrono>
#include <stdlib.h>
#include <stdio.h>
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

ll find_avg_and_std_serial(float*& array, const size_t& size)
{
	double sums[4] = {0.0}, sq_sums[4] = {0.0}, sum, sq_sum, average, standard_deviation;
	size_t i;

	// Start the timer
	auto start_time = high_resolution_clock::now();

	for (i = 0; i < size; i += 4)
	{
		sums[0] += array[i];
		sq_sums[0] += array[i] * array[i];
	}
	for (i = 1; i < size; i += 4)
	{
		sums[1] += array[i];
		sq_sums[1] += array[i] * array[i];
	}
	for (i = 2; i < size; i += 4)
	{
		sums[2] += array[i];
		sq_sums[2] += array[i] * array[i];
	}
	for (i = 3; i < size; i += 4)
	{
		sums[3] += array[i];
		sq_sums[3] += array[i] * array[i];
	}

	sum = sums[0] + sums[1] + sums[2] + sums[3]; 
	sq_sum = sq_sums[0] + sq_sums[1] + sq_sums[2] + sq_sums[3];

	// Stop the timer
	auto finish_time = high_resolution_clock::now();

	average = sum / size;
	standard_deviation = sqrt(sq_sum / size - average * average);

	ll execution_time = duration_cast<nanoseconds>(finish_time - start_time).count();

	// Use a string stream to format the output
	stringstream output_formatter;
	output_formatter.imbue(locale(""));
	output_formatter << execution_time;

	printf("\nSerial Method:\n");
	printf("\t- Average: %.4f\n", average);
	printf("\t- Standard Deviation: %.4f\n", standard_deviation);
	printf("\t- Run Time (ns): %s\n", output_formatter.str().c_str());

	return execution_time;
}

ll find_avg_and_std_parallel(float*& array, const size_t& size)
{
	double sum = 0, sq_sum = 0, average, standard_deviation;
	size_t i;

	__m128 vsum = _mm_setzero_ps();
	__m128 vsq_sum = _mm_setzero_ps();

	// Start the timer
	auto start_time = high_resolution_clock::now();

	for (i = 0; i < size; i += 4)
	{
		__m128 v = _mm_loadu_ps(&array[i]);
		vsum = _mm_add_ps(vsum, v);
		vsq_sum = _mm_add_ps(vsq_sum, _mm_mul_ps(v, v));
	}

	sum = _mm_cvtss_f32(_mm_hadd_ps(_mm_hadd_ps(vsum, vsum), _mm_setzero_ps()));
	sq_sum = _mm_cvtss_f32(_mm_hadd_ps(_mm_hadd_ps(vsq_sum, vsq_sum), _mm_setzero_ps()));

	// Stop the timer
	auto finish_time = high_resolution_clock::now();

	average = sum / size;
	standard_deviation = sqrt(sq_sum / size - average * average);

	ll execution_time = duration_cast<nanoseconds>(finish_time - start_time).count();

	// Use a string stream to format the output
	stringstream output_formatter;
	output_formatter.imbue(locale(""));
	output_formatter << execution_time;

	printf("\nParallel Method:\n");
	printf("\t- Average: %.4f\n", average);
	printf("\t- Standard Deviation: %.4f\n", standard_deviation);
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

	float* array = new float [ARRAY_SIZE];
	generate_random_array(array, ARRAY_SIZE);
    
	ll serial_time = find_avg_and_std_serial(array, ARRAY_SIZE);
	ll parallel_time = find_avg_and_std_parallel(array, ARRAY_SIZE);

	delete[] array;

	printf("\nSpeedup: %.4lf\n", (double) serial_time / (double) parallel_time);

	return 0;
}
