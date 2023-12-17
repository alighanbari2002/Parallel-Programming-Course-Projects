#include <iostream>
#include <random>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <omp.h>
#ifdef 		_WIN32
#include <intrin.h>
#else
#include <x86intrin.h>
#endif

#define ARRAY_SIZE 1048576 // 2 ^ 20
#define NUM_THREADS omp_get_max_threads()

using std::default_random_engine; 
using std::uniform_real_distribution;

void generate_random_array(float*& array, const size_t& size) {
    default_random_engine generator(time(NULL));
    uniform_real_distribution<float> distribution(0.0, pow(10, 6));
    
	for (size_t i = 0; i < size; ++i) {
        array[i] = distribution(generator);
    }
}

double find_min_serial(float*& array, const size_t& size) {
	float min_element = array[0];
	int min_index = 0;
	size_t i;

	double start = omp_get_wtime();

	for (i = 0; i < size; ++i) {
		if (array[i] < min_element) {
			min_element = array[i];
			min_index = i;
		}
	}

	double finish = omp_get_wtime();
	double execution_time = (finish - start) * pow(10, 9);

	printf("\nSerial Method:\n");
	printf("\t- Min Value: %f\n", min_element);
	printf("\t- Min Index: %d\n", min_index);
	printf("\t- Run Time (ns): %.4lf\n", execution_time);

	return execution_time;
}

double find_min_parallel(float*& array, const size_t& size) {
	float min_element = array[0], local_min_element = array[0];
	int min_index = 0, local_min_index = 0;
	size_t i = 0;

	double start = omp_get_wtime();

	#pragma omp parallel default(shared) firstprivate(local_min_element, local_min_index, i) num_threads(NUM_THREADS)
	{
		#pragma omp for simd aligned(array: 64) schedule(auto) nowait
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

	double finish = omp_get_wtime();
	double execution_time = (finish - start) * pow(10, 9);

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
