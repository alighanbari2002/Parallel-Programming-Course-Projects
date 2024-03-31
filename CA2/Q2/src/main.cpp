#include <cstdio>
#include <cstdlib>

#include "functions.hpp"
#include "utils.hpp"

int main()
{
	print_group_info();

	float* array = create_array();
	fill_array_with_random_values(array);

	long long elapsed_time_serial = compute_avg_and_std_serial(array);
	long long elapsed_time_parallel = compute_avg_and_std_parallel(array);

	clean_up_array(array);

	printf("\nSpeedup: %.4lf\n", static_cast<double>(elapsed_time_serial) / elapsed_time_parallel);

	return EXIT_SUCCESS;
}
