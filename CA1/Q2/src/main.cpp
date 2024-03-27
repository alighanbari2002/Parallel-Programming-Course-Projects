#include <cstdio>
#include <cstdlib>
#ifdef 		_WIN32
#include <intrin.h>
#else
#include <x86intrin.h>
#endif

typedef union {
	__m128i 			int128;
	
	unsigned char		m128_u8[16];
	signed char			m128_i8[16];

	unsigned short		m128_u16[8];
	signed short		m128_i16[8];

	u_int32_t			m128_u32[4];
	int32_t				m128_i32[4];

	u_int64_t			m128_u64[4];
	int64_t				m128_i64[4];
} int_vec_t;

typedef union {
	__m128 				f128;

	float				f32[4];
} float_vec_t;

typedef enum {
    u8,
    i8,
    u16,
    i16,
    u32,
    i32,
    u64,
    i64
} data_t;

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

void print_group_info()
{
    printf("Group Members:\n");
	printf("\t- Ali Ghanbari [810199473]\n");
	printf("\t- Behrad Elmi  [810199557]\n");
}

int main()
{
	print_group_info();

	__m128i a;
	unsigned char int_array [16] = { 0X00, 0X11, 0X22, 0X33, 0X44, 0X55, 0X66, 0X77,
									 0X88, 0X99, 0XAA, 0XBB, 0XCC, 0XDD, 0XEE, 0XFF };
	a = _mm_load_si128((const __m128i*)int_array);

	printf("\nTesting \"print_int_vector\" Function:\n");
	print_int_vector(a, u8);
	print_int_vector(a, i8);
	print_int_vector(a, u16);
	print_int_vector(a, i16);
	print_int_vector(a, u32);
	print_int_vector(a, i32);
	print_int_vector(a, u64);
	print_int_vector(a, i64);

	__m128 b;
	float float_array[4] = {-6763.4, -3.1f, 1231231232.5, 7.0f};
	b = _mm_load_ps(float_array);

	printf("\nTesting \"print_spfp_vector\" Function:\n");
	printf("\t- content of the 128-bit register: ");
	print_spfp_vector(b);

	return EXIT_SUCCESS;
}
