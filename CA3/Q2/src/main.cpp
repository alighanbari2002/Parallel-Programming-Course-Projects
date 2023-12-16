#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <math.h>
#include <chrono>
#include <omp.h>
#ifdef 		_WIN32
#include <intrin.h>
#else
#include <x86intrin.h>
#endif

using std::chrono::high_resolution_clock;
using std::chrono::duration_cast;
using std::chrono::nanoseconds;

#define ARRAY_SIZE 1048576 // 2 ^ 20
#define NUM_THREADS 8

void generate_random_array(double* arr, size_t size) {
	double min = 0;
	double max = pow(10, 6);
	double range = max - min;
	
	for (size_t i = 0; i < size; i++) {
		srand(time(NULL));
		float random =  ((double)rand()) / (double)RAND_MAX;
		arr[i] = random * range + min;
	}
}

double find_average_and_std_serial(double* array, size_t size) {
	double sum = 0, average;
	double sum_squared_diff = 0, difference, standard_deviation;
	size_t i;

	auto start = high_resolution_clock::now();

	// Average
	for (i = 0; i < size; i++)
		sum += array[i];
	average = sum / size;

	// Standard Deviation
	for (i = 0; i < size; i++) {
		difference = array[i] - average;
		sum_squared_diff += difference * difference;
	}
	standard_deviation = sqrt(sum_squared_diff / size);

	auto finish = high_resolution_clock::now();
	double execution_time = duration_cast<nanoseconds>(finish - start).count();

	printf("\nSerial Method:\n");
	printf("\t- Average: %.4lf\n", average);
	printf("\t- Standard Deviation: %.4lf\n", standard_deviation);
	printf("\t- Execution Time (ns): %.4lf\n", execution_time);

	return execution_time;
}

double find_average_and_std_parallel(double array[], size_t size) {
	double sum = 0, average;
	double sum_squared_diff = 0, difference, standard_deviation;
	size_t i;

	auto start = high_resolution_clock::now();

	// Average
	#pragma omp parallel for simd num_threads(NUM_THREADS) shared(array) private(i) reduction(+:sum)
		for (i = 0; i < size; i++)
			sum += array[i];
	average = sum / size;

	// Standard Deviation
	#pragma omp parallel for simd num_threads(NUM_THREADS) shared(array, average) private(difference, i) reduction(+:sum_squared_diff)
		for (i = 0; i < size; i++) {
			difference = array[i] - average;
			sum_squared_diff += difference * difference;
		}
	standard_deviation = sqrt(sum_squared_diff / size);

	auto finish = high_resolution_clock::now();
	double execution_time = duration_cast<nanoseconds>(finish - start).count();

	printf("\nParallel Method:\n");
	printf("\t- Average: %.4lf\n", average);
	printf("\t- Standard Deviation: %.4lf\n", standard_deviation);
    printf("\t- Execution Time (ns): %.4lf\n", execution_time);

    return execution_time;
}

void print_group_info() {
    printf("Group Members:\n");
	printf("\t- Ali Ghanbari [810199473]\n");
	printf("\t- Behrad Elmi  [810199557]\n");
}

int main() {
	print_group_info();

	double *array = new double [ARRAY_SIZE];
	generate_random_array(array, ARRAY_SIZE);
    
	double serial_time   = find_average_and_std_serial(array, ARRAY_SIZE);
	double parallel_time = find_average_and_std_parallel(array, ARRAY_SIZE);

	delete array;

	printf("\nSpeedup: %.4lf\n", serial_time / parallel_time);

	return 0;
}
