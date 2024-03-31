#include <cstdio>
#include <cstdlib>

#include "functions.hpp"
#include "utils.hpp"

int main()
{
    print_group_info();

    int cpu_features[4];
    print_processor_info(cpu_features);
    print_cache_info(cpu_features);
    print_SIMD_support(cpu_features);

    return EXIT_SUCCESS;
}
