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

long int find_min_serial(float* array, size_t size) {
	struct timeval start, end;
	float min_elements[4];
	int min_indexes[4];
	float min_element;
	int min_index;

	gettimeofday(&start, NULL);

	min_elements[0] = array[0];
	min_indexes[0] = 0;

	for (size_t i = 0; i < size; i += 4)
	{
		if (array[i] < min_elements[0])
		{
			min_elements[0] = array[i];
			min_indexes[0] = i;
		}
	}

	min_elements[1] = array[1];
	min_indexes[1] = 1;

	for (size_t i = 1; i < size; i += 4)
	{
		if (array[i] < min_elements[1])
		{
			min_elements[1] = array[i];
			min_indexes[1] = i;
		}
	}

	min_elements[2] = array[2];
	min_indexes[2] = 2;

	for (size_t i = 2; i < size; i += 4)
	{
		if (array[i] < min_elements[2])
		{
			min_elements[2] = array[i];
			min_indexes[2] = i;
		}
	}

	min_elements[3] = array[3];
	min_indexes[3] = 3;

	for (size_t i = 3; i < size; i += 4)
	{
		if (array[i] < min_elements[3])
		{
			min_elements[3] = array[i];
			min_indexes[3] = i;
		}
	}

	min_element = min_elements[0];
	min_index = min_indexes[0];
	for (size_t i = 1; i < 4; i++)
	{
		if (min_elements[i] < min_element)
		{
			min_element = min_elements[i];
			min_index = min_indexes[i];
		}
	}

	gettimeofday(&end, NULL);

	long int execution_time = (((end.tv_sec - start.tv_sec) * 1000000) + end.tv_usec) - (start.tv_usec);	
	printf("Serial Method:\n");
	printf("\t- Min value: %f\n", min_element);
	printf("\t- Min index: %d\n", min_index);
	printf("\t- Execution time in microseconds: %ld\n\n", execution_time);

	return execution_time;
}

long int find_min_parallel(float* array, size_t size) {
    struct timeval start, end;

    __m128 min_elements = _mm_set1_ps(FLT_MAX);
    __m128 increment = _mm_set1_ps(4);
    __m128 indexes = _mm_setr_ps(0, 1, 2, 3);
    __m128 min_indexes = _mm_setr_ps(0, 1, 2, 3);
    __m128 value, lt;

    gettimeofday(&start, NULL);

    for (size_t i = 0; i < size; i += 4) {
        value = _mm_loadu_ps(&array[i]);

        lt = _mm_cmpgt_ps(min_elements, value);
        min_elements = _mm_blendv_ps(min_elements, value, lt);
        min_indexes = _mm_blendv_ps(min_indexes, indexes, lt);

        indexes = _mm_add_ps(indexes, increment);
    }

    float array_values[4], array_indexes[4];
    _mm_storeu_ps(array_values, min_elements);
    _mm_storeu_ps(array_indexes, min_indexes);

    float min_element = array_values[0];
    int min_index = (int) array_indexes[0];

    for (size_t i = 1; i < 4; i++) {
        if (array_values[i] < min_element) {
            min_element = array_values[i];
            min_index = (int) array_indexes[i];
        }
    }

    gettimeofday(&end, NULL);
    long int execution_time = (((end.tv_sec - start.tv_sec) * 1000000) + end.tv_usec) - (start.tv_usec);

    printf("Parallel Method:\n");
    printf("\t- Min value: %f\n", min_element);
    printf("\t- Min index: %d\n", min_index);
    printf("\t- Execution time in microseconds: %ld\n\n", execution_time);

    return execution_time;
}

int main() {
	// Show group members
    printf("Group Members:\n");
	printf("\t- Ali Ghanbari [810199473]\n");
	printf("\t- Behrad Elmi  [810199557]\n\n");

	float *array = new float [ARRAY_SIZE];
	generate_random_array(array, ARRAY_SIZE);

	long int serial_execution_time = find_min_serial(array, ARRAY_SIZE);
	long int parallel_execution_time = find_min_parallel(array, ARRAY_SIZE);

	printf("Speed up: %.2f\n", (float)serial_execution_time / (float)parallel_execution_time);
	delete array;
	
	return 0;
}
