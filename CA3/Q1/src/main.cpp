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

void generate_random_array(float* arr, size_t size) {
	float min = 0;
	float max = pow(10, 6);
	float range = max - min;
	
	for (size_t i = 0; i < size; i++) {
		srand(time(NULL));
		float random =  ((float)rand()) / (float)RAND_MAX;
		arr[i] = random * range + min;
	}
}

double find_min_serial(float* array, size_t size) {
	float min_element = array[0];
	int min_index = 0;
	size_t i;

	auto start = high_resolution_clock::now();

	for (i = 0; i < size; i++) {
		if (array[i] < min_element) {
			min_element = array[i];
			min_index = i;
		}
	}

	auto finish = high_resolution_clock::now();
	double execution_time = duration_cast<nanoseconds>(finish - start).count();

	printf("\nSerial Method:\n");
	printf("\t- Min Value: %f\n", min_element);
	printf("\t- Min Index: %d\n", min_index);
	printf("\t- Run Time (ns): %.4lf\n", execution_time);

	return execution_time;
}

double find_min_parallel(float* array, size_t size) {
	float local_min_element, min_element = array[0];
	int local_min_index, min_index = 0;
	size_t i;

	auto start = high_resolution_clock::now();

	#pragma omp parallel num_threads(NUM_THREADS) shared(min_element, min_index) private(i, local_min_element, local_min_index)
	{
		local_min_element = array[0];
    	local_min_index = 0;

		#pragma omp for simd nowait
			for (i = 0; i < size; i++) {
				if (array[i] < local_min_element) {
					local_min_element = array[i];
					local_min_index = i;
				}
			}

		if (local_min_element < min_element) {
			#pragma omp atomic write
				min_element = local_min_element;
			#pragma omp atomic write
				min_index = local_min_index;
		}
	}

	auto finish = high_resolution_clock::now();
	double execution_time = duration_cast<nanoseconds>(finish - start).count();

	printf("\nSerial Method:\n");
	printf("\t- Min Value: %f\n", min_element);
	printf("\t- Min Index: %d\n", min_index);
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

	double serial_time   = find_min_serial(array, ARRAY_SIZE);
	double parallel_time = find_min_parallel(array, ARRAY_SIZE);
	
	delete array;

	printf("\nSpeedup: %.4lf\n", serial_time / parallel_time);
	
	return 0;
}
