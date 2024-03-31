#include <cstdio>
#include <cstddef>
#ifdef _WIN32
#include <intrin.h>
#else
#include <x86intrin.h>
#endif

#include "functions.hpp"
#include "utils.hpp"

void print_spfp_vector(__m128 a)
{
	float_vec_t tmp;
	tmp.f128 = a;
	printf("[");
	for (size_t i = 3; i > 0; i--)
	{
		printf("%f, ", tmp.f32[i]);
	}
	printf("%f]\n", tmp.f32[0]);
}

void print_u8(__m128i a)
{
	int_vec_t tmp;
	tmp.int128 = a;
	printf("[");
	for (size_t i = 15; i > 0; --i)
	{
		printf("%X, ", tmp.m128_u8[i]);
	}
	printf("%X]\n", tmp.m128_u8[0]);
}

void print_i8(__m128i a)
{
	int_vec_t tmp;
	tmp.int128 = a;
	printf("[");
	for (size_t i = 15; i > 0; --i)
	{
		printf("%X, ", tmp.m128_i8[i]);
	}
	printf("%X]\n", tmp.m128_i8[0]);
}

void print_u16(__m128i a)
{
	int_vec_t tmp;
	tmp.int128 = a;
	printf("[");
	for (size_t i = 7; i > 0; --i)
	{
		printf("%X, ", tmp.m128_u16[i]);
	}
	printf("%X]\n", tmp.m128_u16[0]);
}

void print_i16(__m128i a)
{
	int_vec_t tmp;
	tmp.int128 = a;
	printf("[");
	for (size_t i = 7; i > 0; --i)
	{
		printf("%X, ", tmp.m128_i16[i]);
	}
	printf("%X]\n", tmp.m128_i16[0]);
}

void print_u32(__m128i a)
{
	int_vec_t tmp;
	tmp.int128 = a;
	printf("[");
	for (size_t i = 3; i > 0; --i)
	{
		printf("%X, ", tmp.m128_u32[i]);
	}
	printf("%X]\n", tmp.m128_u32[0]);
}

void print_i32(__m128i a)
{
	int_vec_t tmp;
	tmp.int128 = a;
	printf("[");
	for (size_t i = 3; i > 0; --i)
	{
		printf("%X, ", tmp.m128_i32[i]);
	}
	printf("%X]\n", tmp.m128_i32[0]);
}

void print_u64(__m128i a)
{
	int_vec_t tmp;
	tmp.int128 = a;
	printf("[");
	for (size_t i = 1; i > 0; --i)
	{
		printf("%lX, ", tmp.m128_u64[i]);
	}
	printf("%lX]\n", tmp.m128_u64[0]);
}

void print_i64(__m128i a)
{
	int_vec_t tmp;
	tmp.int128 = a;
	printf("[");
	for (size_t i = 1; i > 0; --i)
	{
		printf("%lX, ", tmp.m128_i64[i]);
	}
	printf("%lX]\n", tmp.m128_i64[0]);
}

void print_int_vector(__m128i a, data_t type)
{
    switch (type)
	{
		case u8:
			printf("\t- 16 unsigned bytes format: ");
			print_u8(a);
			break;
		case i8:
			printf("\t- 16 signed bytes format: ");
			print_i8(a);
			break;
		case u16:
			printf("\t- 8 unsigned words format: ");
			print_u16(a);
			break;
		case i16:
			printf("\t- 8 signed words format: ");
			print_i16(a);
			break;
		case u32:
			printf("\t- 4 unsigned double words format: ");
			print_u32(a);
			break;
		case i32:
			printf("\t- 4 signed double words format: ");
			print_i32(a);
			break;
		case u64:
			printf("\t- 2 unsigned quad words format: ");
			print_u64(a);
			break;
		case i64:
			printf("\t- 2 signed quad words format: ");
			print_i64(a);
			break;
		default:
			printf("Invalid type!\n");
			break;
	}
}
