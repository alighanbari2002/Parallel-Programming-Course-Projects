#include <cstdio>
#include <cstdlib>
#ifdef _WIN32
#include <intrin.h>
#else
#include <x86intrin.h>
#endif

#include "functions.hpp"
#include "utils.hpp"

int main()
{
	print_group_info();

	__m128i integer_vector = _mm_load_si128(reinterpret_cast<const __m128i*>(utils::sample_values::HEX_VALUES));

	printf("\nTesting \"print_int_vector\" Function:\n");
	print_int_vector(integer_vector, u8);
	print_int_vector(integer_vector, i8);
	print_int_vector(integer_vector, u16);
	print_int_vector(integer_vector, i16);
	print_int_vector(integer_vector, u32);
	print_int_vector(integer_vector, i32);
	print_int_vector(integer_vector, u64);
	print_int_vector(integer_vector, i64);

	__m128 float_vector = _mm_load_ps(utils::sample_values::FLOATS);

	printf("\nTesting \"print_spfp_vector\" Function:\n");
	printf("\t- content of the 128-bit register: ");
	print_spfp_vector(float_vector);

	return EXIT_SUCCESS;
}
