#ifndef UTILS_HPP
#define UTILS_HPP

using DWORD = unsigned long int;

namespace utils {

    namespace hardware_constants {
        constexpr unsigned int INITIAL_APIC_ID_BITS = 0xff000000;
        constexpr unsigned int HWD_MT_BIT = 1 << 28;
        constexpr unsigned int NUM_CORE_BITS = 0xfc000000;
        constexpr unsigned int NUM_LOGICAL_BITS = 0xFF0000;
    }  // namespace hardware_constants

} // namespace utils

#ifdef _WIN32 // Windows
#define cpuid __cpuid
#else         // Linux
void cpuid(int cpu_info[4], int info_type);
#endif

void print_group_info();

#endif // UTILS_HPP
