#include <cstdio>
#include <cstdlib>
#include <string>
#include <cmath>
#include <vector>
#include <pthread.h>

#include "functions.hpp"
#include "utils.hpp"

long long compute_avg_and_std_serial(const float* const &array)
{
	double sum = 0, sq_sum = 0, average, standard_deviation;

	// Start the timer
	auto start_time = get_current_time();

    size_t i;

	for (i = 0; i < utils::config::ARRAY_SIZE; ++i)
	{
		sum += array[i];
		sq_sum += array[i] * array[i];
	}

	// Stop the timer
	auto finish_time = get_current_time();

	average = sum / utils::config::ARRAY_SIZE;
	standard_deviation = std::sqrt(sq_sum / utils::config::ARRAY_SIZE - average * average);

	long long elapsed_time = calculate_duration(start_time, finish_time);

	printf("\nSerial Method:\n");
	printf("\t- Average: %.4lf\n", average);
	printf("\t- Standard Deviation: %.4lf\n", standard_deviation);
	printf("\t- Run Time (ns): %s\n", format_time(elapsed_time).c_str());

	return elapsed_time;
}

void* calculate_sums(void* arg)
{
    thread_data_t* data = (thread_data_t*) arg;

    double local_sum = 0;
    double local_sq_sum = 0;

    size_t i;

    for (i = data->start; i < data->end; ++i)
    {
        local_sum += data->array[i];
        local_sq_sum += data->array[i] * data->array[i];
    }

    data->sum = local_sum;
    data->sq_sum = local_sq_sum;

    pthread_exit(nullptr);
}

long long compute_avg_and_std_parallel(const float* const &array, const size_t& num_threads)
{
    double sum = 0, sq_sum = 0, average, standard_deviation;

    std::vector<pthread_t> threads(num_threads);
    std::vector<thread_data_t> thread_args(num_threads);

    size_t chunk_size = utils::config::ARRAY_SIZE / num_threads;

	size_t i;

	// Start the timer
	auto start_time = get_current_time();

    // Assign the arguments for each thread
    for (i = 0; i < num_threads; ++i)
    {
        thread_args[i].array = array;

        thread_args[i].start = i * chunk_size;
        thread_args[i].end = (i == num_threads - 1) ? utils::config::ARRAY_SIZE : (i + 1) * chunk_size;

        int rc = pthread_create(&threads[i], NULL, calculate_sums, static_cast<void*>(&thread_args[i]));
        if (rc)
        {
            printf("ERROR: return code from pthread_create() is %d\n", rc);
            exit(EXIT_FAILURE);
        }
    }

    // Wait for all the threads to finish and collect their results
    for (i = 0; i < num_threads; ++i)
    {
        int rc = pthread_join(threads[i], NULL);
        if (rc)
        {
            printf("ERROR: return code from pthread_join() is %d\n", rc);
            exit(EXIT_FAILURE);
        }

        sum += thread_args[i].sum;
        sq_sum += thread_args[i].sq_sum;
    }

	// Stop the timer
	auto finish_time = get_current_time();

	average = sum / utils::config::ARRAY_SIZE;
	standard_deviation = std::sqrt(sq_sum / utils::config::ARRAY_SIZE - average * average);

	long long elapsed_time = calculate_duration(start_time, finish_time);

	printf("\nParallel Method:\n");
	printf("\t- Average: %.4lf\n", average);
	printf("\t- Standard Deviation: %.4lf\n", standard_deviation);
	printf("\t- Run Time (ns): %s\n", format_time(elapsed_time).c_str());

	return elapsed_time;
}