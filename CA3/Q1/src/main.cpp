#include <iostream>
#include <random>
#include <sstream>
#include <cstdio>
#include <cstdlib>
#include <cfloat>
#include <omp.h>

typedef struct {
	float value;
	int index;
} array_element_t;

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

long long min_search_serial(const float* const &array, const size_t& array_size)
{
	array_element_t min_element = {array[0], 0};

	size_t i;

	// Start the timer
	double start_time = get_current_time();

	for (i = 1; i < array_size; ++i)
	{
		if (array[i] < min_element.value)
		{
			min_element.value = array[i];
			min_element.index = i;
		}
	}

	// Stop the timer
	double finish_time = get_current_time();

	long long elapsed_time = calculate_duration(start_time, finish_time);

	printf("\nSerial Method:\n");
	printf("\t- Min Value: %f\n", min_element.value);
	printf("\t- Min Index: %d\n", min_element.index);
	printf("\t- Run Time (ns): %s\n", format_time(elapsed_time));

	return elapsed_time;
}

long long min_search_parallel(const float* const &array, const size_t& array_size)
{
	#pragma omp declare reduction(minimum : array_element_t : \
			omp_out = omp_in.value < omp_out.value ? omp_in : omp_out) \
			initializer(omp_priv = {FLT_MAX, -1})

	array_element_t min_element = {array[0], 0};

	size_t i;

	// Start the timer
	double start_time = get_current_time();

	#pragma omp parallel for simd default(shared) private(i) \
			reduction(minimum:min_element) schedule(static)
		for (i = 1; i < array_size; ++i)
		{
			if (array[i] < min_element.value)
			{
				min_element.value = array[i];
				min_element.index = i;
			}
		}

	// Stop the timer
	double finish_time = get_current_time();

	long long elapsed_time = calculate_duration(start_time, finish_time);

	printf("\nParallel Method:\n");
	printf("\t- Min Value: %f\n", min_element.value);
	printf("\t- Min Index: %d\n", min_element.index);
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

	long long elapsed_time_serial = min_search_serial(array, ARRAY_SIZE);
	long long elapsed_time_parallel = min_search_parallel(array, ARRAY_SIZE);

	clean_up_array(array);

	printf("\nSpeedup: %.4lf\n", static_cast<double>(elapsed_time_serial) / elapsed_time_parallel);

	return EXIT_SUCCESS;
}
