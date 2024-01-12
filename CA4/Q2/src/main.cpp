#include <iostream>
#include <random>
#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <math.h>
#include <unistd.h>
#include <chrono>
#include <vector>
#include <algorithm>

using std::default_random_engine; 
using std::uniform_real_distribution;
using std::chrono::high_resolution_clock;
using std::chrono::duration_cast;
using std::chrono::nanoseconds;

typedef struct {
    std::vector<double>::iterator start, end;
    double* sum;
    double* sq_sum;
    pthread_mutex_t* sum_mutex;
    pthread_mutex_t* sq_sum_mutex;
} ThreadData;

#define ARRAY_SIZE 1048576 // 2 ^ 20
#define NUM_THREADS static_cast<size_t>(sysconf(_SC_NPROCESSORS_ONLN) / 4)
#define CHUNK_SIZE (ARRAY_SIZE / NUM_THREADS)

void generate_random_array(std::vector<double>& array, const size_t& size)
{
    default_random_engine generator(time(NULL));
    uniform_real_distribution<double> distribution(0.0, pow(10, 6));

	for (size_t i = 0; i < size; ++i)
    {
        array[i] = distribution(generator);
    }
}

double find_average_and_std_serial(std::vector<double>& array, const size_t& size)
{
	double sum = 0, sq_sum = 0, average, standard_deviation;
	size_t i;

	auto start = high_resolution_clock::now();

	for (i = 0; i < size; ++i)
    {
		sum += array[i];
        sq_sum += array[i] * array[i];
    }
	average = sum / size;
	standard_deviation = sqrt(sq_sum / size - average * average);

	auto finish = high_resolution_clock::now();
	double execution_time = duration_cast<nanoseconds>(finish - start).count();

	printf("\nSerial Method:\n");
	printf("\t- Average: %.4lf\n", average);
	printf("\t- Standard Deviation: %.4lf\n", standard_deviation);
	printf("\t- Run Time (ns): %.4lf\n", execution_time);

	return execution_time;
}

void* find_average_and_std_thread(void* arg)
{
    ThreadData* data = (ThreadData*)arg;
    double local_sum = 0, local_sq_sum = 0;

    for(auto it = data->start; it != data->end; ++it)
    {
        local_sum += *it;
        local_sq_sum += (*it) * (*it);
    }

    pthread_mutex_lock(data->sum_mutex);
    *(data->sum) += local_sum;
    pthread_mutex_unlock(data->sum_mutex);

    pthread_mutex_lock(data->sq_sum_mutex);
    *(data->sq_sum) += local_sq_sum;
    pthread_mutex_unlock(data->sq_sum_mutex);

    pthread_exit(NULL);
}

double find_average_and_std_parallel(std::vector<double>& array)
{
    double sum = 0, sq_sum = 0, average, standard_deviation;

    auto start = high_resolution_clock::now();

    pthread_t threads[NUM_THREADS];
    ThreadData thread_data_array[NUM_THREADS];
    pthread_mutex_t sum_mutex, sq_sum_mutex;
    pthread_mutex_init(&sum_mutex, NULL);
    pthread_mutex_init(&sq_sum_mutex, NULL);

    for(size_t i = 0; i < NUM_THREADS; ++i) {
        thread_data_array[i].start = array.begin() + i * CHUNK_SIZE;
        thread_data_array[i].end = (i == NUM_THREADS - 1 ? array.end() : thread_data_array[i].start + CHUNK_SIZE);
        thread_data_array[i].sum = &sum;
        thread_data_array[i].sq_sum = &sq_sum;
        thread_data_array[i].sum_mutex = &sum_mutex;
        thread_data_array[i].sq_sum_mutex = &sq_sum_mutex;

        pthread_create(&threads[i], NULL, find_average_and_std_thread, &thread_data_array[i]);
    }

    for(size_t i = 0; i < NUM_THREADS; ++i) {
        pthread_join(threads[i], NULL);
    }

    average = sum / array.size();
    standard_deviation = sqrt(sq_sum / array.size() - average * average);

    auto finish = high_resolution_clock::now();
    double execution_time = duration_cast<nanoseconds>(finish - start).count();

    printf("\nParallel Method:\n");
    printf("\t- Average: %.4lf\n", average);
    printf("\t- Standard Deviation: %.4lf\n", standard_deviation);
    printf("\t- Run Time (ns): %.4lf\n", execution_time);

    pthread_mutex_destroy(&sum_mutex);
    pthread_mutex_destroy(&sq_sum_mutex);

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

	std::vector<double> array(ARRAY_SIZE);
	generate_random_array(array, ARRAY_SIZE);
    
	double serial_time   = find_average_and_std_serial(array, ARRAY_SIZE);
	double parallel_time = find_average_and_std_parallel(array);

	printf("\nSpeedup: %.4lf\n", serial_time / parallel_time);

	return 0;
}