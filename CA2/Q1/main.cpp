#include <stdio.h>
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
		float random = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX));
		arr[i] = random * range + min;
	}
}

int main() {
	// Show group members
    printf("Group Members:\n");
	printf("\t- Ali Ghanbari [810199473]\n");
	printf("\t- Behrad Elmi  [810199557]\n");

	printf("%f", pow(10, 6));

	float *array = new float [ARRAY_SIZE];
	generate_random_array(array, ARRAY_SIZE);
	
    return 0;
}