#include <iostream>
#include <random>
#include <sstream>
#include <cstdio>
#include <cstdlib>
#include <cmath>
#include <omp.h>

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

double get_current_time()
{
	return omp_get_wtime();
}

long long calculate_duration(const double& start_time, const double& finish_time)
{
	return static_cast<long long>((finish_time - start_time) * 1e9);
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

	size_t i;

	// Start the timer
	double start_time = get_current_time();

	for (i = 0; i < array_size; ++i)
	{
		sum += array[i];
		sq_sum += array[i] * array[i];
	}

	// Stop the timer
	double finish_time = get_current_time();

	average = sum / array_size;
	standard_deviation = std::sqrt(sq_sum / array_size - average * average);

	long long elapsed_time = calculate_duration(start_time, finish_time);

	printf("\nSerial Method:\n");
	printf("\t- Average: %.4lf\n", average);
	printf("\t- Standard Deviation: %.4lf\n", standard_deviation);
	printf("\t- Run Time (ns): %s\n", format_time(elapsed_time));

	return elapsed_time;
}

long long compute_avg_and_std_parallel(const float* const &array, const size_t& array_size)
{
	double sum = 0, sq_sum = 0, average, standard_deviation;

	size_t i;

	// Start the timer
	double start_time = get_current_time();

	#pragma omp parallel for simd default(shared) private(i) \
			reduction(+:sum,sq_sum) schedule(static)
		for (i = 0; i < array_size; ++i)
		{
			sum += array[i];
			sq_sum += array[i] * array[i];
		}

	// Stop the timer
	double finish_time = get_current_time();

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

	// Set the number of threads
	size_t num_threads = omp_get_max_threads() - 1;
	omp_set_num_threads(num_threads);

	long long elapsed_time_serial = compute_avg_and_std_serial(array, ARRAY_SIZE);
	long long elapsed_time_parallel = compute_avg_and_std_parallel(array, ARRAY_SIZE);

	clean_up_array(array);

	printf("\nSpeedup: %.4lf\n", static_cast<double>(elapsed_time_serial) / elapsed_time_parallel);

	return EXIT_SUCCESS;
}
