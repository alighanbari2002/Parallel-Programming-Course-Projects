#include <iostream>
#include <random>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <unistd.h>
#include <chrono>
#include <pthread.h>

using std::default_random_engine; 
using std::uniform_real_distribution;
using std::chrono::high_resolution_clock;
using std::chrono::duration_cast;
using std::chrono::nanoseconds;

typedef struct
{
    double* arr;
    size_t start, end;
    double min_element;
    int min_index;
} ThreadData;

#define ARRAY_SIZE 1048576 // 2 ^ 20
#define NUM_THREADS static_cast<size_t>(sysconf(_SC_NPROCESSORS_ONLN) - 1)
#define CHUNK_SIZE (ARRAY_SIZE / NUM_THREADS)

void generate_random_array(double*& array, const size_t& size)
{
    default_random_engine generator(time(NULL));
    uniform_real_distribution<double> distribution(0.0, pow(10, 6));

	for (size_t i = 0; i < size; ++i)
    {
        array[i] = distribution(generator);
    }
}

double find_min_serial(double*& array, const size_t& size)
{
	double min_element = array[0];
	int min_index = 0;
	size_t i;

	auto start = high_resolution_clock::now();

	for (i = 0; i < size; ++i)
    {
		if (array[i] < min_element)
        {
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

void* find_min_thread(void* arg)
{
    ThreadData* data = (ThreadData*)arg;
    data->min_index = data->start;
    data->min_element = data->arr[data->start];

    for(size_t i = data->start + 1; i < data->end; ++i)
    {
        if(data->arr[i] < data->min_element)
        {
            data->min_index = i;
            data->min_element = data->arr[i];
        }
    }

    pthread_exit(NULL);
}

double find_min_parallel(double*& array, const size_t& size)
{
    double min_element = array[0];
	int min_index = 0;
	size_t i;
 
    pthread_t threads[NUM_THREADS];
    ThreadData thread_data_array[NUM_THREADS];

	auto start = high_resolution_clock::now();

    // Loop through the number of threads and create each thread
    for(i = 0; i < NUM_THREADS; ++i)
    {
        thread_data_array[i].arr = array;
        thread_data_array[i].start = i * CHUNK_SIZE;
        thread_data_array[i].end = (i + 1) * CHUNK_SIZE;

        int rc = pthread_create(&threads[i], NULL, find_min_thread, &thread_data_array[i]);
        if (rc)
        {
            printf("ERROR: return code from pthread_create() is %d\n", rc);
            exit(-1);
        }
    }

    // Loop through the number of threads and join each thread
    for(i = 0; i < NUM_THREADS; ++i)
    {
        void *status;
        int rc = pthread_join(threads[i], &status);
        if (rc)
        {
            printf("ERROR: return code from pthread_join() is %d\n", rc);
            exit(-1);
        }

        if (thread_data_array[i].min_element < min_element)
        {
            min_index = thread_data_array[i].min_index;
            min_element = thread_data_array[i].min_element;
        }
    }

	auto finish = high_resolution_clock::now();
	double execution_time = duration_cast<nanoseconds>(finish - start).count();

    printf("\nParallel Method:\n");
	printf("\t- Min Value: %f\n", min_element);
	printf("\t- Min Index: %d\n", min_index);
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

	double serial_time   = find_min_serial(array, ARRAY_SIZE);
	double parallel_time = find_min_parallel(array, ARRAY_SIZE);
	
	delete array;

	printf("\nSpeedup: %.4lf\n", serial_time / parallel_time);
	
	return 0;
}
