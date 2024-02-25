#include <iostream>
#include <random>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <chrono>
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

double find_average_and_std_serial(float*& array, const size_t& size)
{
	auto start = high_resolution_clock::now();

	double sums[4] = {0.0}, sq_sums[4] = {0.0}, average, standard_deviation;
	size_t i;

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

	average = (sums[0] + sums[1] + sums[2] + sums[3]) / size;
	standard_deviation = sqrt((sq_sums[0] + sq_sums[1] + sq_sums[2] + sq_sums[3]) / size - average * average);

	auto finish = high_resolution_clock::now();
	double execution_time = duration_cast<nanoseconds>(finish - start).count();

	printf("\nSerial Method:\n");
	printf("\t- Average: %.4f\n", average);
	printf("\t- Standard Deviation: %.4f\n", standard_deviation);
	printf("\t- Run Time (ns): %.4lf\n", execution_time);

	return execution_time;
}

double find_average_and_std_parallel(float*& array, const size_t& size)
{
	auto start = high_resolution_clock::now();

	double sum = 0, sq_sum = 0, average, standard_deviation;
	size_t i;

	__m128 vsum = _mm_setzero_ps();
	__m128 vsq_sum = _mm_setzero_ps();

	for (i = 0; i < size; i += 4)
	{
		__m128 v = _mm_loadu_ps(&array[i]);
		vsum = _mm_add_ps(vsum, v);
		vsq_sum = _mm_add_ps(vsq_sum, _mm_mul_ps(v, v));
	}

	sum = _mm_cvtss_f32(_mm_hadd_ps(_mm_hadd_ps(vsum, vsum), _mm_setzero_ps()));
	sq_sum = _mm_cvtss_f32(_mm_hadd_ps(_mm_hadd_ps(vsq_sum, vsq_sum), _mm_setzero_ps()));

	average = sum / size;
	standard_deviation = sqrt(sq_sum / size - average * average);

	auto finish = high_resolution_clock::now();
	double execution_time = duration_cast<nanoseconds>(finish - start).count();

	printf("\nParallel Method:\n");
	printf("\t- Average: %.4f\n", average);
	printf("\t- Standard Deviation: %.4f\n", standard_deviation);
    printf("\t- Run Time (ns): %.4lf\n", execution_time);

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
    
	double serial_time   = find_average_and_std_serial(array, ARRAY_SIZE);
	double parallel_time = find_average_and_std_parallel(array, ARRAY_SIZE);

	delete array;

	printf("\nSpeedup: %.4lf\n", serial_time / parallel_time);

	return 0;
}
