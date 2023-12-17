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

void generate_random_array(double*& array, const size_t& size) {
    default_random_engine generator(time(NULL));
    uniform_real_distribution<double> distribution(0.0, pow(10, 6));
    
	for (size_t i = 0; i < size; ++i) {
        array[i] = distribution(generator);
    }
}

double find_average_and_std_serial(double*& array, const size_t& size) {
	double sum = 0, average;
	double sum_squared_diff = 0, difference, standard_deviation;
	size_t i;

	double start = omp_get_wtime();

	// Average
	for (i = 0; i < size; ++i)
		sum += array[i];
	average = sum / size;

	// Standard Deviation
	for (i = 0; i < size; ++i) {
		difference = array[i] - average;
		sum_squared_diff += difference * difference;
	}
	standard_deviation = sqrt(sum_squared_diff / size);

	double finish = omp_get_wtime();
	double execution_time = (finish - start) * pow(10, 9);

	printf("\nSerial Method:\n");
	printf("\t- Average: %.4lf\n", average);
	printf("\t- Standard Deviation: %.4lf\n", standard_deviation);
	printf("\t- Run Time (ns): %.4lf\n", execution_time);

	return execution_time;
}

double find_average_and_std_parallel(double*& array, const size_t& size) {
	double sum = 0, average;
	double sum_squared_diff = 0, difference, standard_deviation;
	size_t i;

	double start = omp_get_wtime();

	// Average
	#pragma omp parallel for simd default(shared) private(i) reduction(+:sum) \
			schedule(auto) num_threads(NUM_THREADS)
		for (i = 0; i < size; ++i)
			sum += array[i];
	average = sum / size;

	// Standard Deviation
	#pragma omp parallel for simd default(shared) private(difference, i) \
			reduction(+:sum_squared_diff) schedule(auto) num_threads(NUM_THREADS)
		for (i = 0; i < size; ++i) {
			difference = array[i] - average;
			sum_squared_diff += difference * difference;
		}
	standard_deviation = sqrt(sum_squared_diff / size);

	double finish = omp_get_wtime();
	double execution_time = (finish - start) * pow(10, 9);

	printf("\nParallel Method:\n");
	printf("\t- Average: %.4lf\n", average);
	printf("\t- Standard Deviation: %.4lf\n", standard_deviation);
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

	double *array = new double [ARRAY_SIZE];
	generate_random_array(array, ARRAY_SIZE);
    
	double serial_time   = find_average_and_std_serial(array, ARRAY_SIZE);
	double parallel_time = find_average_and_std_parallel(array, ARRAY_SIZE);

	delete array;

	printf("\nSpeedup: %.4lf\n", serial_time / parallel_time);

	return 0;
}
