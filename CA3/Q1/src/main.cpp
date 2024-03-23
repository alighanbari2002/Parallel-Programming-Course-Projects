#include <iostream>
#include <random>
#include <sstream>
#include <stdlib.h>
#include <stdio.h>
#include <float.h>
#include <omp.h>

using std::default_random_engine; 
using std::uniform_real_distribution;
using std::stringstream;
using std::locale;

typedef struct {
	float value;
	int index;
} array_element_t;

typedef long long ll;

#define ARRAY_SIZE 1048576 // 2 ^ 20

void generate_random_array(float*& array, const size_t& size)
{
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<> dist(0, 1e7);

	for (size_t i = 0; i < size; ++i)
	{
		array[i] = dist(gen);
	}
}

ll min_search_serial(float*& array, const size_t& size)
{
	array_element_t min_element = {array[0], 0};
	size_t i;

	// Start the timer
	double start_time = omp_get_wtime();

	for (i = 1; i < size; ++i)
	{
		if (array[i] < min_element.value)
		{
			min_element.value = array[i];
			min_element.index = i;
		}
	}

	// Stop the timer
	double finish_time = omp_get_wtime();

	ll execution_time = (finish_time - start_time) * 1e9;

	// Use a string stream to format the output
	stringstream output_formatter;
	output_formatter.imbue(locale(""));
	output_formatter << execution_time;

	printf("\nSerial Method:\n");
	printf("\t- Min Value: %f\n", min_element.value);
	printf("\t- Min Index: %d\n", min_element.index);
	printf("\t- Run Time (ns): %s\n", output_formatter.str().c_str());

	return execution_time;
}

ll min_search_parallel(float*& array, const size_t& size)
{
	#pragma omp declare reduction(minimum : array_element_t : \
			omp_out = omp_in.value < omp_out.value ? omp_in : omp_out) \
			initializer(omp_priv = {FLT_MAX, -1})

	array_element_t min_element = {array[0], 0};
	size_t i;

	int num_threads = omp_get_max_threads();
	omp_set_num_threads(num_threads - 1);

	// Start the timer
	double start_time = omp_get_wtime();

	#pragma omp parallel for simd default(shared) private(i) \
			reduction(minimum:min_element) schedule(static)
		for (i = 1; i < size; ++i)
		{
			if (array[i] < min_element.value)
			{
				min_element.value = array[i];
				min_element.index = i;
			}
		}

	// Stop the timer
	double finish_time = omp_get_wtime();

	ll execution_time = (finish_time - start_time) * 1e9;

	// Use a string stream to format the output
	stringstream output_formatter;
	output_formatter.imbue(locale(""));
	output_formatter << execution_time;

	printf("\nParallel Method:\n");
	printf("\t- Min Value: %f\n", min_element.value);
	printf("\t- Min Index: %d\n", min_element.index);
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

	ll serial_time = min_search_serial(array, ARRAY_SIZE);
	ll parallel_time = min_search_parallel(array, ARRAY_SIZE);
	
	delete[] array;

	printf("\nSpeedup: %.4lf\n", (double) serial_time / (double) parallel_time);
	
	return 0;
}
