#include <iostream>
#include <sstream>
#include <random>
#include <stdlib.h>
#include <stdio.h>
#include <time.h>
#include <math.h>
#include <omp.h>

using std::default_random_engine; 
using std::uniform_real_distribution;
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

ll min_search_serial(float*& array, const size_t& size)
{
	float min_element = array[0];
	int min_index = 0;
	size_t i;

	// Start the timer
	double start_time = omp_get_wtime();

	for (i = 0; i < size; ++i)
	{
		if (array[i] < min_element)
		{
			min_element = array[i];
			min_index = i;
		}
	}

	// Stop the timer
	double finish_time = omp_get_wtime();

	ll execution_time = (finish_time - start_time) * pow(10, 9);

	// Use a string stream to format the output
	stringstream output_formatter;
	output_formatter.imbue(locale(""));
	output_formatter << execution_time;

	printf("\nSerial Method:\n");
	printf("\t- Min Value: %f\n", min_element);
	printf("\t- Min Index: %d\n", min_index);
	printf("\t- Run Time (ns): %s\n", output_formatter.str().c_str());

	return execution_time;
}

ll min_search_parallel(float*& array, const size_t& size)
{
	float min_element = array[0], local_min_element = array[0];
	int min_index = 0, local_min_index = 0;
	size_t i = 0;

	int num_threads = omp_get_max_threads();
	omp_set_num_threads(num_threads - 1);

	// Start the timer
	double start_time = omp_get_wtime();

	#pragma omp parallel default(shared) firstprivate(local_min_element, local_min_index, i)
	{
		#pragma omp for simd aligned(array: 64) schedule(auto) nowait
			for (i = 0; i < size; i++)
			{
				if (array[i] < local_min_element)
				{
					local_min_element = array[i];
					local_min_index = i;
				}
			}

		if (local_min_element < min_element)
		{
			#pragma omp atomic write
				min_element = local_min_element;
			#pragma omp atomic write
				min_index = local_min_index;
		}
	}

	// Stop the timer
	double finish_time = omp_get_wtime();

	ll execution_time = (finish_time - start_time) * pow(10, 9);

	// Use a string stream to format the output
	stringstream output_formatter;
	output_formatter.imbue(locale(""));
	output_formatter << execution_time;

	printf("\nParallel Method:\n");
	printf("\t- Min Value: %f\n", min_element);
	printf("\t- Min Index: %d\n", min_index);
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

	float *array = new float [ARRAY_SIZE];
	generate_random_array(array, ARRAY_SIZE);

	ll serial_time = min_search_serial(array, ARRAY_SIZE);
	ll parallel_time = min_search_parallel(array, ARRAY_SIZE);
	
	delete array;

	printf("\nSpeedup: %.4lf\n", (double) serial_time / (double) parallel_time);
	
	return 0;
}
