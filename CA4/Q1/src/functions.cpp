#include <cstdio>
#include <cstdlib>
#include <string>
#include <vector>
#include <pthread.h>

#include "functions.hpp"
#include "utils.hpp"

long long min_search_serial(const float* const &array)
{
	float min_element = array[0];
	int min_index = 0;

	// Start the timer
	auto start_time = get_current_time();

    size_t i;

	for (i = 1; i < utils::config::ARRAY_SIZE; ++i)
    {
		if (array[i] < min_element)
        {
			min_element = array[i];
			min_index = i;
		}
	}

	// Stop the timer
	auto finish_time = get_current_time();

	long long elapsed_time = calculate_duration(start_time, finish_time);

	printf("\nSerial Method:\n");
	printf("\t- Min Value: %f\n", min_element);
	printf("\t- Min Index: %d\n", min_index);
	printf("\t- Run Time (ns): %s\n", format_time(elapsed_time).c_str());

	return elapsed_time;
}

void* find_local_min(void* arg)
{
    thread_data_t* data = (thread_data_t*) arg;

    float local_min_element = data->array[data->start];
    int local_min_index = data->start;

    size_t i;

    for (i = data->start + 1; i < data->end; ++i)
    {
        if (data->array[i] < local_min_element)
        {
            local_min_element = data->array[i];
            local_min_index = i;
        }
    }

    data->min_element = local_min_element;
    data->min_index = local_min_index;

    pthread_exit(nullptr);
}

long long min_search_parallel(const float* const &array, const size_t& num_threads)
{
    float min_element = array[0];
	int min_index = 0;

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

        int rc = pthread_create(&threads[i], NULL, find_local_min, static_cast<void*>(&thread_args[i]));
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

        if (thread_args[i].min_element < min_element)
        {
            min_element = thread_args[i].min_element;
            min_index = thread_args[i].min_index;
        }
    }

	// Stop the timer
	auto finish_time = get_current_time();

	long long elapsed_time = calculate_duration(start_time, finish_time);

	printf("\nParallel Method:\n");
	printf("\t- Min Value: %f\n", min_element);
	printf("\t- Min Index: %d\n", min_index);
	printf("\t- Run Time (ns): %s\n", format_time(elapsed_time).c_str());

	return elapsed_time;
}
