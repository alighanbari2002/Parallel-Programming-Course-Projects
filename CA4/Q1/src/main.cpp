#include <iostream>
#include <random>
#include <sstream>
#include <cstdio>
#include <cstdlib>
#include <chrono>
#include <thread>
#include <unistd.h>
#include <pthread.h>

typedef struct {
    const float* array;
    size_t start;
    size_t end;
    float min_element;
    int min_index;
} thread_data_t;

void print_group_info()
{
    printf("Group Members:\n");
    printf("\t- Ali Ghanbari [810199473]\n");
    printf("\t- Behrad Elmi  [810199557]\n");
}

float* create_array(const size_t& array_size)
{
	return new float[array_size];
}

void clean_up_array(float*& array)
{
    delete[] array;
	array = nullptr;
}

std::chrono::high_resolution_clock::time_point get_current_time()
{
    return std::chrono::high_resolution_clock::now();
}

template <typename T>
long long calculate_duration(const T& start_time, const T& finish_time)
{
    return std::chrono::duration_cast<std::chrono::nanoseconds>(finish_time - start_time).count();
}

const char* format_time(const long long& time_ns)
{
    std::stringstream time_formatter;
    time_formatter.imbue(std::locale(""));
    time_formatter << time_ns;
    return time_formatter.str().c_str();
}

void fill_array_with_random_values(float*& array, const size_t& array_size)
{
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_real_distribution<float> distr(0.0f, 1e7);

	for (size_t i = 0; i < array_size; ++i)
	{
		array[i] = distr(gen);
	}
}

long long min_search_serial(const float* const &array, const size_t& array_size)
{
	float min_element = array[0];
	int min_index = 0;

	// Start the timer
	auto start_time = get_current_time();

    size_t i;

	for (i = 1; i < array_size; ++i)
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
	printf("\t- Run Time (ns): %s\n", format_time(elapsed_time));

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

long long min_search_parallel(const float* const &array, const size_t& array_size, const size_t& num_threads)
{
    float min_element = array[0];
	int min_index = 0;

    pthread_t threads[num_threads];
    thread_data_t thread_args[num_threads];

    size_t chunk_size = array_size / num_threads;

	size_t i;
 
	// Start the timer
	auto start_time = get_current_time();

    // Assign the arguments for each thread
    for (i = 0; i < num_threads; ++i)
    {
        thread_args[i].array = array;

        thread_args[i].start = i * chunk_size;
        thread_args[i].end = (i == num_threads - 1) ? array_size : (i + 1) * chunk_size;

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
	printf("\t- Run Time (ns): %s\n", format_time(elapsed_time));

	return elapsed_time;
}

int main()
{
	print_group_info();

	const size_t ARRAY_SIZE = 1048576; // 2 ^ 20
	float* array = create_array(ARRAY_SIZE);
	fill_array_with_random_values(array, ARRAY_SIZE);

	long long elapsed_time_serial = min_search_serial(array, ARRAY_SIZE);
	
	// Set the number of threads
    size_t num_threads = std::thread::hardware_concurrency() - 1;

    long long elapsed_time_parallel = min_search_parallel(array, ARRAY_SIZE, num_threads);

	clean_up_array(array);

	printf("\nSpeedup: %.4lf\n", static_cast<double>(elapsed_time_serial) / elapsed_time_parallel);

    pthread_exit(nullptr);
}
