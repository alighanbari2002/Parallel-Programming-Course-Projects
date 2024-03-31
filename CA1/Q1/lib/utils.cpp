#include <cstdio>

#include "utils.hpp"

#ifndef _WIN32 // Linux
void cpuid(int cpu_info[4], int info_type)
{
	__asm__ __volatile__(
		"cpuid":
		"=a" (cpu_info[0]),
		"=b" (cpu_info[1]),
		"=c" (cpu_info[2]),
		"=d" (cpu_info[3]) :
		"a" (info_type)
		);
}
#endif

void print_group_info()
{
    printf("Group Members:\n");
	printf("\t- Ali Ghanbari [810199473]\n");
	printf("\t- Behrad Elmi  [810199557]\n");
}
