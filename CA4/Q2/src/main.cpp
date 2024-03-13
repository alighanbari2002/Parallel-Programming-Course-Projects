#include <iostream>
#include <random>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <unistd.h>
#include <chrono>
#include <sstream>
#include <algorithm>
#include <pthread.h>

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
    double sum;
    double sq_sum;
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

ll find_average_and_std_serial(double*& array, const size_t& size)
{
	double sum = 0, sq_sum = 0, average, standard_deviation;

	// Start the timer
	auto start = high_resolution_clock::now();

	for (size_t i = 0; i < size; ++i)
    {
		sum += array[i];
        sq_sum += array[i] * array[i];
    }

	// Stop the timer
	auto finish = high_resolution_clock::now();

	average = sum / size;
	standard_deviation = sqrt(sq_sum / size - average * average);

	ll execution_time = duration_cast<nanoseconds>(finish - start).count();

	// Use a string stream to format the output
	stringstream ss;
	ss.imbue(locale(""));
	ss << execution_time;

	printf("\nSerial Method:\n");
	printf("\t- Average: %.4lf\n", average);
	printf("\t- Standard Deviation: %.4lf\n", standard_deviation);
	printf("\t- Run Time (ns): %s\n", ss.str().c_str());

	return execution_time;
}

void* average_and_std_thread(void* arg)
{
    thread_data_t* data = (thread_data_t*)arg;

    double local_sum = 0;
    double local_sq_sum = 0;

    for (size_t i = data->start; i < data->end; ++i)
    {
        local_sum += data->array[i];
        local_sq_sum += data->array[i] * data->array[i];
    }

    data->sum = local_sum;
    data->sq_sum = local_sq_sum;

    pthread_exit(NULL);
}

ll find_average_and_std_parallel(double*& array, const size_t& size)
{
    double sum = 0, sq_sum = 0, average, standard_deviation;

    int num_threads = sysconf(_SC_NPROCESSORS_ONLN) - 1;

    pthread_t threads[num_threads];
    thread_data_t thread_data_array[num_threads];

    size_t chunk_size = size / num_threads;

	int i;

	// Start the timer
	auto start = high_resolution_clock::now();

    // Assign the arguments for each thread
    for (i = 0; i < num_threads; ++i)
    {
        thread_data_array[i].array = array;

        thread_data_array[i].start = i * chunk_size;
        thread_data_array[i].end = (i == num_threads - 1) ? size : (i + 1) * chunk_size;

        int rc = pthread_create(&threads[i], NULL, average_and_std_thread, &thread_data_array[i]);
        if (rc)
        {
            printf("ERROR: return code from pthread_create() is %d\n", rc);
            exit(-1);
        }
    }

    // Wait for all the threads to finish and collect their results
    for (i = 0; i < num_threads; ++i)
    {
        int rc = pthread_join(threads[i], NULL);
        if (rc)
        {
            printf("ERROR: return code from pthread_join() is %d\n", rc);
            exit(-1);
        }

        sum += thread_data_array[i].sum;
        sq_sum += thread_data_array[i].sq_sum;
    }

	// Stop the timer
	auto finish = high_resolution_clock::now();

    average = sum / size;
    standard_deviation = sqrt(sq_sum / size - average * average);

	ll execution_time = duration_cast<nanoseconds>(finish - start).count();

	// Use a string stream to format the output
	stringstream ss;
	ss.imbue(locale(""));
	ss << execution_time;

    printf("\nParallel Method:\n");
    printf("\t- Average: %.4lf\n", average);
    printf("\t- Standard Deviation: %.4lf\n", standard_deviation);
	printf("\t- Run Time (ns): %s\n", ss.str().c_str());

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

	ll serial_time = find_average_and_std_serial(array, ARRAY_SIZE);
	ll parallel_time = find_average_and_std_parallel(array, ARRAY_SIZE);

	delete array;

	printf("\nSpeedup: %.4lf\n", (double) serial_time / (double) parallel_time);

	return 0;
}
