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
	printf("\t- Behrad Elmi  [810199557]\n");
    
	printf("\nProcessor Info:\n");
	int CPUInfo[4];

    // Get processor type
	int processorType[12];
	cpuid(processorType + 0x0, 0x80000002);
	cpuid(processorType + 0x4, 0x80000003);
	cpuid(processorType + 0x8, 0x80000004);
	printf("\t- Type: %s\n", (char*)processorType);

    // Get count of physical cores
    cpuid(CPUInfo, 0x4);
    int physicalCores = ((CPUInfo[0] >> 26) & 0x3F) + 1;
    printf("\t- Physical Cores: %d\n", physicalCores);

    // Get count of logical cores
    cpuid(CPUInfo, 0x1);
    int logicalCores = (CPUInfo[1] >> 16) & 0xFF;
    printf("\t- Logical Cores: %d\n", logicalCores);

    // Check if hyperthreading is supported
	cpuid(CPUInfo, 0x1);
    int isHyperthreadingSupported = (CPUInfo[2] >> 28) & 0x1;
    if (isHyperthreadingSupported) {
        printf("\t- Hyperthreading is supported.\n");
    } else {
        printf("\t- Hyperthreading is not supported.\n");
    }

	printf("\nCache Info:\n");

	// Get cache information for cache level 1
    cpuid(CPUInfo, 0x4 | (1 << 5)); // Set ECX to 1 to get cache level 1 information

    // Extract cache type and cache size
    int cacheType = CPUInfo[0] & 0x1F;
    int cacheSize = ((CPUInfo[1] >> 22) & 0x3FF) * 1024; // Size in KB

    printf("\t- Type: %d\n", cacheType);
    printf("\t- Size: %d KB\n", cacheSize);

    return 0;
}