#include <stdio.h>
#include <x86intrin.h>

#ifdef 		_WIN32					//  Windows
#define 	cpuid    __cpuid
#else								//  Linux

void cpuid(int CPUInfo[4], int InfoType) {
	__asm__ __volatile__(
		"cpuid":
		"=a" (CPUInfo[0]),
		"=b" (CPUInfo[1]),
		"=c" (CPUInfo[2]),
		"=d" (CPUInfo[3]) :
		"a" (InfoType)
		);
}
#endif

int main() {
    printf("\n");
    printf("\t/-----------------------------\\\n");
    printf("\t| Group Members:              |\n");
    printf("\t|  - Ali Ghanbari %% 810199473 |\n");
    printf("\t|  - Behrad Elmi  %% 810199557 |\n");
    printf("\t\\-----------------------------/\n");
    printf("\n");

    int CPUInfo[4];
    
    // Get processor type
    cpuid(CPUInfo, 0x80000002);
    printf("Processor Type: %s\n", (char*)CPUInfo);

    return 0;
}