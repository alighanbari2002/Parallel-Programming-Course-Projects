#ifndef FUNCTIONS_HPP
#define FUNCTIONS_HPP

#ifdef _WIN32
#include <intrin.h>
#else
#include <x86intrin.h>
#endif

#include "utils.hpp"

void print_spfp_vector(__m128 a);

void print_u8(__m128i a);

void print_i8(__m128i a);

void print_u16(__m128i a);

void print_i16(__m128i a);

void print_u32(__m128i a);

void print_i32(__m128i a);

void print_u64(__m128i a);

void print_i64(__m128i a);

void print_int_vector(__m128i a, data_t type);

#endif // FUNCTIONS_HPP
