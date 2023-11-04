#include <stdio.h>
#include <stdlib.h>
// #include <ipp.h>
#include <sys/time.h>
#include <time.h>
#include <float.h>
#include <math.h>
#ifdef 		_WIN32
#include <intrin.h>
#else
#include <x86intrin.h>
#endif
#include <ctime>

#define ARRAY_SIZE 1048576

void generate_random_array(float* arr, size_t size) {
	float min = 0;
	float max = pow(10, 6); 
	float range = max - min;
	
	for (size_t i = 0; i < size; i++) {
		srand(time(NULL));
		float random =  ((float)rand()) / (float)RAND_MAX;
		arr[i] = random * range + min;
	}
}

long int find_average_and_std_serial(float* array, size_t size) {
	struct timeval start, end;
	
	clock_t sserial, eserial;
	gettimeofday(&start, NULL);
	sserial = clock();

	float sums[4] = {0}, average;
	for (size_t i = 0; i < size; i += 4)
		sums[0] += array[i];
	for (size_t i = 1; i < size; i += 4)
		sums[1] += array[i];
	for (size_t i = 2; i < size; i += 4)
		sums[2] += array[i];
	for (size_t i = 3; i < size; i += 4)
		sums[3] += array[i];
	average = ((sums[0] + sums[1]) + (sums[2] + sums[3])) / size;
	
	sums[0] = sums[1] = sums[2] = sums[3] = 0;
	float standard_deviation, difference;
	for (size_t i = 0; i < size; i += 4)
	{
		difference = array[i] - average;
		sums[0] += difference * difference;
	}
	for (size_t i = 1; i < size; i += 4)
	{
		difference = array[i] - average;
		sums[1] += difference * difference;
	}
	for (size_t i = 2; i < size; i += 4)
	{
		difference = array[i] - average;
		sums[2] += difference * difference;
	}
	for (size_t i = 3; i < size; i += 4)
	{
		difference = array[i] - average;
		sums[3] += difference * difference;
	}
	standard_deviation = sqrt(((sums[3] + sums[2]) + (sums[1] + sums[0])) / size);

	eserial = clock();
	gettimeofday(&end, NULL);

	long int execution_time = (((end.tv_sec - start.tv_sec) * 1000000) + end.tv_usec) - (start.tv_usec);	
	printf("Serial Method:\n");
	printf("\t- Average: %f\n", average);
	printf("\t- Standard Deviation: %f\n", standard_deviation);
	printf("\t- Execution time in microseconds: %ld\n\n", execution_time);

	return eserial-sserial;
}

long int find_average_and_std_parallel(float array[], size_t size) {
	struct timeval start, end;
	
	clock_t sparallel, eparallel;
	gettimeofday(&start, NULL);
	sparallel = clock();

	// Average
	float average;
	__m128 value;
	__m128 sum = _mm_set1_ps(0);
	for (size_t i = 0; i < size; i += 4)
	{
		value = _mm_loadu_ps(&array[i]);
		sum = _mm_add_ps(sum, value);
	}
	sum = _mm_hadd_ps(sum, sum);
	sum = _mm_hadd_ps(sum, sum);
	average = (_mm_cvtss_f32(sum)) / size;

	// Standard Deviation
	float standard_deviation;
	sum = _mm_set1_ps(0);
	__m128 average_register = _mm_set1_ps(average);
	for (size_t i = 0; i < size; i += 4)
	{
		value = _mm_loadu_ps(&array[i]);
		value = _mm_sub_ps(value, average_register);
		value = _mm_mul_ps(value, value);
		sum = _mm_add_ps(sum, value);
	}
	sum = _mm_hadd_ps(sum, sum);
	sum = _mm_hadd_ps(sum, sum);
	standard_deviation = (_mm_cvtss_f32(sum)) / size;
	standard_deviation = sqrt(standard_deviation);

	eparallel = clock();
	gettimeofday(&end, NULL);

	long int execution_time = (((end.tv_sec - start.tv_sec) * 1000000) + end.tv_usec) - (start.tv_usec);
	printf("Parallel Method:\n");
	printf("\tAverage: %f\n", average);
	printf("\tStandard Deviation: %f\n", standard_deviation);
	printf("\tExecution time in microseconds: %ld\n\n", execution_time);
	return eparallel-sparallel;
}

int main() {
	// Show group members
    printf("Group Members:\n");
	printf("\t- Ali Ghanbari [810199473]\n");
	printf("\t- Behrad Elmi  [810199557]\n\n");

	float *array = new float [ARRAY_SIZE];
	generate_random_array(array, ARRAY_SIZE);

    
	long int serial_clocks = find_average_and_std_serial(array, ARRAY_SIZE);
	long int parallel_clocks = find_average_and_std_parallel(array, ARRAY_SIZE);

	printf("Speedup:\n\t"
       "- %0.4f\n", (float)(serial_clocks) / (float)((parallel_clocks)));	
    delete array;

	return 0;
}
