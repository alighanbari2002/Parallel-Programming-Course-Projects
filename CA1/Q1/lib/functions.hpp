#ifndef FUNCTIONS_HPP
#define FUNCTIONS_HPP

void print_SIMD_support_status(int (&info)[4]);

void print_SIMD_support(int (&info)[4]);

void get_cache_info(int (&info)[4], int& cache_type, int& cache_size);

void print_cache_info(int (&info)[4]);

bool is_hyperthreading_supported(int (&info)[4]);

unsigned int cpuid_supported();

unsigned int genuine_intel();

unsigned int is_HWMT_supported();

unsigned int get_max_num_cores_per_package();

unsigned int get_max_numLP_per_package();

unsigned find_mask_width(unsigned int count_item);

unsigned char get_initial_apic_ID();

unsigned char get_nzb_subID(unsigned char fullID, unsigned char max_subID_value, unsigned char shift_count);

int get_num_active_cores();

int get_num_logical_cores(int (&info)[4]);

int get_num_physical_cores(int (&info)[4]);

void print_processor_info(int (&info)[4]);

#endif // FUNCTIONS_HPP
