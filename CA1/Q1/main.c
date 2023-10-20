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
	// Show group members
    printf("Group Members:\n");
	printf("\t- Ali Ghanbari [810199473]\n");
	printf("\t- Behrad Elmi [810199557]\n");
    printf("\n");
    
    // Get processor type
	int processorType[12];
	cpuid(processorType + 0x0, 0x80000002);
	cpuid(processorType + 0x4, 0x80000003);
	cpuid(processorType + 0x8, 0x80000004);
	printf("Processor Type:\n\t%s\n", (char*)processorType);

    return 0;
}