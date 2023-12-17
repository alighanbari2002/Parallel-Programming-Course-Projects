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

void generate_random_array(float*& array, const size_t& size) {
    default_random_engine generator(time(NULL));
    uniform_real_distribution<float> distribution(0.0, pow(10, 6));
    
	for (size_t i = 0; i < size; ++i) {
        array[i] = distribution(generator);
    }
}

double find_average_and_std_serial(float*& array, const size_t& size) {
	auto start = high_resolution_clock::now();

	// Average
	float sums[4] = {0.0}, average;
	for (size_t i = 0; i < size; i += 4)
		sums[0] += array[i];
	for (size_t i = 1; i < size; i += 4)
		sums[1] += array[i];
	for (size_t i = 2; i < size; i += 4)
		sums[2] += array[i];
	for (size_t i = 3; i < size; i += 4)
		sums[3] += array[i];
	average = ((sums[0] + sums[1]) + (sums[2] + sums[3])) / size;

	// Standard Deviation
	sums[0] = sums[1] = sums[2] = sums[3] = 0.0;
	float standard_deviation, difference;
	for (size_t i = 0; i < size; i += 4) {
		difference = array[i] - average;
		sums[0] += difference * difference;
	}
	for (size_t i = 1; i < size; i += 4) {
		difference = array[i] - average;
		sums[1] += difference * difference;
	}
	for (size_t i = 2; i < size; i += 4) {
		difference = array[i] - average;
		sums[2] += difference * difference;
	}
	for (size_t i = 3; i < size; i += 4) {
		difference = array[i] - average;
		sums[3] += difference * difference;
	}
	standard_deviation = sqrt(((sums[3] + sums[2]) + (sums[1] + sums[0])) / size);

	auto finish = high_resolution_clock::now();
	double execution_time = duration_cast<nanoseconds>(finish - start).count();

	printf("\nSerial Method:\n");
	printf("\t- Average: %.4f\n", average);
	printf("\t- Standard Deviation: %.4f\n", standard_deviation);
	printf("\t- Run Time (ns): %.4lf\n", execution_time);

	return execution_time;
}

double find_average_and_std_parallel(float*& array, const size_t& size) {
	auto start = high_resolution_clock::now();

	// Average
	float average;
	__m128 value;
	__m128 sum = _mm_set1_ps(0);
	for (size_t i = 0; i < size; i += 4) {
		value = _mm_loadu_ps(&array[i]);
		sum = _mm_add_ps(sum, value);
	}
	sum = _mm_hadd_ps(sum, sum);
	sum = _mm_hadd_ps(sum, sum);
	average = (_mm_cvtss_f32(sum)) / size;

	// Standard Deviation
	float standard_deviation;
	sum = _mm_set1_ps(0);
	__m128 average_register = _mm_set1_ps(average);
	for (size_t i = 0; i < size; i += 4) {
		value = _mm_loadu_ps(&array[i]);
		value = _mm_sub_ps(value, average_register);
		value = _mm_mul_ps(value, value);
		sum = _mm_add_ps(sum, value);
	}
	sum = _mm_hadd_ps(sum, sum);
	sum = _mm_hadd_ps(sum, sum);
	standard_deviation = (_mm_cvtss_f32(sum)) / size;
	standard_deviation = sqrt(standard_deviation);

	auto finish = high_resolution_clock::now();
	double execution_time = duration_cast<nanoseconds>(finish - start).count();

	printf("\nParallel Method:\n");
	printf("\t- Average: %.4f\n", average);
	printf("\t- Standard Deviation: %.4f\n", standard_deviation);
    printf("\t- Run Time (ns): %.4lf\n", execution_time);

    return execution_time;
}

void print_group_info() {
    printf("Group Members:\n");
	printf("\t- Ali Ghanbari [810199473]\n");
	printf("\t- Behrad Elmi  [810199557]\n");
}

int main() {
	print_group_info();

	float *array = new float [ARRAY_SIZE];
	generate_random_array(array, ARRAY_SIZE);
    
	double serial_time   = find_average_and_std_serial(array, ARRAY_SIZE);
	double parallel_time = find_average_and_std_parallel(array, ARRAY_SIZE);

	delete array;

	printf("\nSpeedup: %.4lf\n", serial_time / parallel_time);

	return 0;
}
