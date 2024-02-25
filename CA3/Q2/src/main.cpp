#include <iostream>
#include <random>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <omp.h>

#define ARRAY_SIZE 1048576 // 2 ^ 20

using std::default_random_engine; 
using std::uniform_real_distribution;

void generate_random_array(double*& array, const size_t& size)
{
	default_random_engine generator(time(NULL));
	uniform_real_distribution<double> distribution(0.0, pow(10, 6));

	for (size_t i = 0; i < size; ++i)
	{
		array[i] = distribution(generator);
	}
}

double find_average_and_std_serial(double*& array, const size_t& size)
{
	double sum = 0, sq_sum = 0, average, standard_deviation;
	size_t i;

	// Start the timer
	double start = omp_get_wtime();

	for (i = 0; i < size; ++i)
	{
		sum += array[i];
		sq_sum += array[i] * array[i];
	}

	// Stop the timer
	double finish = omp_get_wtime();

	average = sum / size;
	standard_deviation = sqrt(sq_sum / size - average * average);

	double execution_time = (finish - start) * pow(10, 9);

	printf("\nSerial Method:\n");
	printf("\t- Average: %.4lf\n", average);
	printf("\t- Standard Deviation: %.4lf\n", standard_deviation);
	printf("\t- Run Time (ns): %.4lf\n", execution_time);

	return execution_time;
}

double find_average_and_std_parallel(double*& array, const size_t& size)
{
	double sum = 0, sq_sum = 0, average, standard_deviation;
	size_t i;

	// Start the timer
	double start = omp_get_wtime();

	#pragma omp parallel for simd default(shared) private(i) \
			reduction(+:sum,sq_sum) schedule(auto)
		for (i = 0; i < size; ++i)
		{
			sum += array[i];
			sq_sum += array[i] * array[i];
		}

	// Stop the timer
	double finish = omp_get_wtime();

	average = sum / size;
	standard_deviation = sqrt(sq_sum / size - average * average);

	double execution_time = (finish - start) * pow(10, 9);

	printf("\nParallel Method:\n");
	printf("\t- Average: %.4lf\n", average);
	printf("\t- Standard Deviation: %.4lf\n", standard_deviation);
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

	double *array = new double [ARRAY_SIZE];
	generate_random_array(array, ARRAY_SIZE);

	int num_threads = omp_get_max_threads() - 1;
	omp_set_num_threads(num_threads);

	double serial_time   = find_average_and_std_serial(array, ARRAY_SIZE);
	double parallel_time = find_average_and_std_parallel(array, ARRAY_SIZE);

	delete array;

	printf("\nSpeedup: %.4lf\n", serial_time / parallel_time);

	return 0;
}
