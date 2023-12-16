#include <stdio.h>
#include <stdlib.h>
#include <float.h>
#include <math.h>
#include <chrono>
#ifdef 		_WIN32
#include <intrin.h>
#else
#include <x86intrin.h>
#endif

using std::chrono::high_resolution_clock;
using std::chrono::duration_cast;
using std::chrono::nanoseconds;

#define ARRAY_SIZE 1048576 // 2 ^ 20

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

double find_min_serial(float* array, size_t size) {
	float min_elements[4];
	int min_indexes[4];
	float min_element;
	int min_index;

	auto start = high_resolution_clock::now();

	min_elements[0] = array[0];
	min_indexes[0] = 0;

	for (size_t i = 0; i < size; i += 4) {
		if (array[i] < min_elements[0]) {
			min_elements[0] = array[i];
			min_indexes[0] = i;
		}
	}

	min_elements[1] = array[1];
	min_indexes[1] = 1;

	for (size_t i = 1; i < size; i += 4) {
		if (array[i] < min_elements[1]) {
			min_elements[1] = array[i];
			min_indexes[1] = i;
		}
	}

	min_elements[2] = array[2];
	min_indexes[2] = 2;

	for (size_t i = 2; i < size; i += 4) {
		if (array[i] < min_elements[2]) {
			min_elements[2] = array[i];
			min_indexes[2] = i;
		}
	}

	min_elements[3] = array[3];
	min_indexes[3] = 3;

	for (size_t i = 3; i < size; i += 4) {
		if (array[i] < min_elements[3]) {
			min_elements[3] = array[i];
			min_indexes[3] = i;
		}
	}

	min_element = min_elements[0];
	min_index = min_indexes[0];
	for (size_t i = 1; i < 4; i++) {
		if (min_elements[i] < min_element) {
			min_element = min_elements[i];
			min_index = min_indexes[i];
		}
	}

	auto finish = high_resolution_clock::now();
	double execution_time = duration_cast<nanoseconds>(finish - start).count();

	printf("\nSerial Method:\n");
	printf("\t- Min Value: %f\n", min_element);
	printf("\t- Min Index: %d\n", min_index);
	printf("\t- Run Time (ns): %.4lf\n", execution_time);

	return execution_time;
}

double find_min_parallel(float* array, size_t size) {
    __m128 min_elements = _mm_set1_ps(FLT_MAX);
    __m128 increment = _mm_set1_ps(4);
    __m128 indexes = _mm_setr_ps(0, 1, 2, 3);
    __m128 min_indexes = _mm_setr_ps(0, 1, 2, 3);
    __m128 value, lt;

	auto start = high_resolution_clock::now();

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

	auto finish = high_resolution_clock::now();
	double execution_time = duration_cast<nanoseconds>(finish - start).count();

    printf("\nParallel Method:\n");
    printf("\t- Min Value: %f\n", min_element);
    printf("\t- Min Index: %d\n", min_index);
    printf("\t- Run Time (ns): %.4lf\n", execution_time);

    return execution_time;
}

void print_group_info() {
    printf("Group Members:\n");
	printf("\t- Ali Ghanbari [810199473]\n");
	printf("\t- Behrad Elmi  [810199557]\n");
}

int main() {
    print_group_info();

	float *array = new float [ARRAY_SIZE];
	generate_random_array(array, ARRAY_SIZE);

	double serial_time   = find_min_serial(array, ARRAY_SIZE);
	double parallel_time = find_min_parallel(array, ARRAY_SIZE);
	
	delete array;

	printf("\nSpeedup: %.4lf\n", serial_time / parallel_time);
	
	return 0;
}
