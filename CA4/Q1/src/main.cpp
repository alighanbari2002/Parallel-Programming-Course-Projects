#include <iostream>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <unistd.h>
#include <pthread.h>
#include <sstream>
#include <random>
#include <chrono>

using std::default_random_engine; 
using std::uniform_real_distribution;
using std::chrono::high_resolution_clock;
using std::chrono::duration_cast;
using std::chrono::nanoseconds;
using std::stringstream;
using std::locale;

typedef long long ll;

typedef struct {
    double* array;
    size_t start;
    size_t end;
    double min_element;
    int min_index;
} thread_data_t;

#define ARRAY_SIZE 1048576 // 2 ^ 20

void generate_random_array(double*& array, const size_t& size)
{
    default_random_engine generator(time(NULL));
    uniform_real_distribution<double> distribution(0.0, pow(10, 6));

    for (size_t i = 0; i < size; ++i)
    {
        array[i] = distribution(generator);
    }
}

ll min_search_serial(double*& array, const size_t& size)
{
	double min_element = array[0];
	int min_index = 0;

	// Start the timer
	auto start_time = high_resolution_clock::now();

	for (size_t i = 0; i < size; ++i)
    {
		if (array[i] < min_element)
        {
			min_element = array[i];
			min_index = i;
		}
	}

	// Stop the timer
	auto finish_time = high_resolution_clock::now();

	ll execution_time = duration_cast<nanoseconds>(finish_time - start_time).count();

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

void* find_local_min(void* arg)
{
    thread_data_t* data = (thread_data_t*) arg;

    double local_min_element = data->array[data->start];
    int local_min_index = data->start;

    for (size_t i = data->start + 1; i <= data->end; ++i)
    {
        if (data->array[i] < local_min_element)
        {
            local_min_element = data->array[i];
            local_min_index = i;
        }
    }

    data->min_element = local_min_element;
    data->min_index = local_min_index;

    pthread_exit(NULL);
}

ll min_search_parallel(double*& array, const size_t& size)
{
    double min_element = array[0];
	int min_index = 0;

    int num_procs = sysconf(_SC_NPROCESSORS_ONLN);
    int num_threads = num_procs - 1;

    pthread_t threads[num_threads];
    thread_data_t thread_data_array[num_threads];

    size_t chunk_size = size / num_threads;

	int i;
 
	// Start the timer
	auto start_time = high_resolution_clock::now();

    // Assign the arguments for each thread
    for (i = 0; i < num_threads; ++i)
    {
        thread_data_array[i].array = array;

        thread_data_array[i].start = i * chunk_size;
        thread_data_array[i].end = (i == num_threads - 1) ? (size - 1) : (thread_data_array[i].start + chunk_size - 1);

        int rc = pthread_create(&threads[i], NULL, find_local_min, &thread_data_array[i]);
        if (rc)
        {
            printf("ERROR: return code from pthread_create() is %d\n", rc);
            exit(-1);
        }
    }

    // Wait for all the threads to finish and collect their results
    for(i = 0; i < num_threads; ++i)
    {
        int rc = pthread_join(threads[i], NULL);
        if (rc)
        {
            printf("ERROR: return code from pthread_join() is %d\n", rc);
            exit(-1);
        }

        if (thread_data_array[i].min_element < min_element)
        {
            min_element = thread_data_array[i].min_element;
            min_index = thread_data_array[i].min_index;
        }
    }

	// Stop the timer
	auto finish_time = high_resolution_clock::now();

	ll execution_time = duration_cast<nanoseconds>(finish_time - start_time).count();

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

	double* array = new double [ARRAY_SIZE];
	generate_random_array(array, ARRAY_SIZE);

	ll serial_time = min_search_serial(array, ARRAY_SIZE);
	ll parallel_time = min_search_parallel(array, ARRAY_SIZE);

	delete[] array;

	printf("\nSpeedup: %.4lf\n", (double) serial_time / (double) parallel_time);

	return 0;
}
