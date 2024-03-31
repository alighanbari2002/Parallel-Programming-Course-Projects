#include <cstdio>
#include <cstdlib>
#include <thread>
#include <pthread.h>

#include "functions.hpp"
#include "utils.hpp"

int main()
{
	print_group_info();

	float* array = create_array();
	fill_array_with_random_values(array);

	long long elapsed_time_serial = compute_avg_and_std_serial(array);

	// Set the number of threads
	size_t num_threads = std::thread::hardware_concurrency() - 1;

	long long elapsed_time_parallel = compute_avg_and_std_parallel(array, num_threads);

	clean_up_array(array);

	printf("\nSpeedup: %.4lf\n", static_cast<double>(elapsed_time_serial) / elapsed_time_parallel);

    pthread_exit(nullptr);
}
