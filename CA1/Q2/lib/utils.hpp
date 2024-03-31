#ifndef UTILS_HPP
#define UTILS_HPP

#ifdef _WIN32
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

namespace utils {

	namespace sample_values {
		constexpr unsigned char HEX_VALUES[16] = {
			0x00, 0x11, 0x22, 0x33, 0x44, 0x55, 0x66, 0x77,
			0x88, 0x99, 0xAA, 0xBB, 0xCC, 0xDD, 0xEE, 0xFF
		};
		constexpr float FLOATS[4] = {-6763.4f, -3.1f, 1231231232.5f, 7.0f};
	}; // namespace sample_values

} // namespace utils

void print_group_info();

#endif // UTILS_HPP
