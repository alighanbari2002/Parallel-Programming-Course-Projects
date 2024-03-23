#include <iostream>
#include <random>
#include <sstream>
#include <stdlib.h>
#include <stdio.h>
#include <omp.h>

using std::default_random_engine; 
using std::uniform_real_distribution;
using std::stringstream;
using std::locale;

typedef long long ll;

#define ARRAY_SIZE 1048576 // 2 ^ 20

void generate_random_array(double*& array, const size_t& size)
{
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<double> dist(0, 1e7);

	for (size_t i = 0; i < size; ++i)
	{
		array[i] = dist(gen);
	}
}

ll find_avg_and_std_serial(double*& array, const size_t& size)
{
	double sum = 0, sq_sum = 0, average, standard_deviation;
	size_t i;

	// Start the timer
	double start_time = omp_get_wtime();

	for (i = 0; i < size; ++i)
	{
		sum += array[i];
		sq_sum += array[i] * array[i];
	}

	// Stop the timer
	double finish_time = omp_get_wtime();

	average = sum / size;
	standard_deviation = sqrt(sq_sum / size - average * average);

	ll execution_time = (finish_time - start_time) * 1e9;

	// Use a string stream to format the output
	stringstream output_formatter;
	output_formatter.imbue(locale(""));
	output_formatter << execution_time;

	printf("\nSerial Method:\n");
	printf("\t- Average: %.4lf\n", average);
	printf("\t- Standard Deviation: %.4lf\n", standard_deviation);
	printf("\t- Run Time (ns): %s\n", output_formatter.str().c_str());

	return execution_time;
}

ll find_avg_and_std_parallel(double*& array, const size_t& size)
{
	double sum = 0, sq_sum = 0, average, standard_deviation;
	size_t i;

	int num_threads = omp_get_max_threads();
	omp_set_num_threads(num_threads - 1);

	// Start the timer
	double start_time = omp_get_wtime();

	#pragma omp parallel for simd default(shared) private(i) \
			reduction(+:sum,sq_sum) schedule(static)
		for (i = 0; i < size; ++i)
		{
			sum += array[i];
			sq_sum += array[i] * array[i];
		}

	// Stop the timer
	double finish_time = omp_get_wtime();

	average = sum / size;
	standard_deviation = sqrt(sq_sum / size - average * average);

	ll execution_time = (finish_time - start_time) * 1e9;

	// Use a string stream to format the output
	stringstream output_formatter;
	output_formatter.imbue(locale(""));
	output_formatter << execution_time;

	printf("\nParallel Method:\n");
	printf("\t- Average: %.4lf\n", average);
	printf("\t- Standard Deviation: %.4lf\n", standard_deviation);
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

	double* array = new double [ARRAY_SIZE];
	generate_random_array(array, ARRAY_SIZE);

	ll serial_time = find_avg_and_std_serial(array, ARRAY_SIZE);
	ll parallel_time = find_avg_and_std_parallel(array, ARRAY_SIZE);

	delete[] array;

	printf("\nSpeedup: %.4lf\n", (double) serial_time / (double) parallel_time);

	return 0;
}
