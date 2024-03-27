#include <iostream>
#include <random>
#include <sstream>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <chrono>
#include <thread>
#include <unistd.h>
#include <pthread.h>

typedef struct {
    const float* array;
    size_t start;
    size_t end;
    double sum;
    double sq_sum;
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

long long compute_avg_and_std_serial(const float* const &array, const size_t& array_size)
{
	double sum = 0, sq_sum = 0, average, standard_deviation;

	// Start the timer
	auto start_time = get_current_time();

    size_t i;

	for (i = 0; i < array_size; ++i)
	{
		sum += array[i];
		sq_sum += array[i] * array[i];
	}

	// Stop the timer
	auto finish_time = get_current_time();

	average = sum / array_size;
	standard_deviation = std::sqrt(sq_sum / array_size - average * average);

	long long elapsed_time = calculate_duration(start_time, finish_time);

	printf("\nSerial Method:\n");
	printf("\t- Average: %.4lf\n", average);
	printf("\t- Standard Deviation: %.4lf\n", standard_deviation);
	printf("\t- Run Time (ns): %s\n", format_time(elapsed_time));

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

long long compute_avg_and_std_parallel(const float* const &array, const size_t& array_size, const size_t& num_threads)
{
    double sum = 0, sq_sum = 0, average, standard_deviation;

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

	average = sum / array_size;
	standard_deviation = std::sqrt(sq_sum / array_size - average * average);

	long long elapsed_time = calculate_duration(start_time, finish_time);

	printf("\nParallel Method:\n");
	printf("\t- Average: %.4lf\n", average);
	printf("\t- Standard Deviation: %.4lf\n", standard_deviation);
	printf("\t- Run Time (ns): %s\n", format_time(elapsed_time));

	return elapsed_time;
}

int main()
{
	print_group_info();

	const size_t ARRAY_SIZE = 1048576; // 2 ^ 20
	float* array = create_array(ARRAY_SIZE);
	fill_array_with_random_values(array, ARRAY_SIZE);

	long long elapsed_time_serial = compute_avg_and_std_serial(array, ARRAY_SIZE);
	
	// Set the number of threads
    size_t num_threads = std::thread::hardware_concurrency() - 1;

	long long elapsed_time_parallel = compute_avg_and_std_parallel(array, ARRAY_SIZE, num_threads);

	clean_up_array(array);

	printf("\nSpeedup: %.4lf\n", static_cast<double>(elapsed_time_serial) / elapsed_time_parallel);

    pthread_exit(nullptr);
}
