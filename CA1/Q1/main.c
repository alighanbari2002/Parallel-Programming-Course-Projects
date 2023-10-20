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
	int info[4];

    // Get processor type
	int processorType[12];
	cpuid(processorType + 0x0, 0x80000002);
	cpuid(processorType + 0x4, 0x80000003);
	cpuid(processorType + 0x8, 0x80000004);
	printf("\t- Type: %s\n", (char*)processorType);

    // Get count of physical cores
    cpuid(info, 0x4);
    int physicalCores = ((info[0] >> 26) & 0x3F) + 1;
    printf("\t- Physical Cores: %d\n", physicalCores);

    // Get count of logical cores
    cpuid(info, 0x1);
    int logicalCores = (info[1] >> 16) & 0xFF;
    printf("\t- Logical Cores: %d\n", logicalCores);

    // Check if hyperthreading is supported
	cpuid(info, 0x1);
    int isHyperthreadingSupported = (info[2] >> 28) & 0x1;
    if (isHyperthreadingSupported) {
        printf("\t- Hyperthreading is supported.\n");
    } else {
        printf("\t- Hyperthreading is not supported.\n");
    }

	printf("\nCache Info:\n");

	// Get cache information for cache level 1
    cpuid(info, 0x4 | (1 << 5)); // Set ECX to 1 to get cache level 1 information
    // Extract cache type and cache size
    int cacheType = info[0] & 0x1F;
    int cacheSize = ((info[1] >> 22) & 0x3FF) * 1024; // Size in KB
    printf("\t- Type: %d\n", cacheType);
    printf("\t- Size: %d KB\n", cacheSize);

	printf("\nSIMD Architecture Support:\n");

    // Check for SIMD architecture support (MMX, SSE, SSE2, SSE3, etc.)
    bool MMX   = false;
	bool SSE   = false;
	bool SSE2  = false;
	bool SSE3  = false;
	bool AES   = false;
	bool SSE41 = false;
	bool SSE42 = false;
	bool AVX   = false;
	bool AVX2  = false;
	bool SHA   = false;

    cpuid(info, 0x00000001);
	MMX   = (info[3] & ((int)1 << 23)) != 0;
	SSE   = (info[3] & ((int)1 << 25)) != 0;
	SSE2  = (info[3] & ((int)1 << 26)) != 0;
	SSE3  = (info[2] & ((int)1 << 0))  != 0;
	AES   = (info[2] & ((int)1 << 25)) != 0;
	SSE41 = (info[2] & ((int)1 << 19)) != 0;
	SSE42 = (info[2] & ((int)1 << 20)) != 0;
	AVX   = (info[2] & ((int)1 << 28)) != 0;
	cpuid(info, 0x80000000);
	if (info[0] >= 0x00000007){
		cpuid(info, 0x00000007);
		AVX2   = (info[1] & ((int)1 <<  5)) != 0;
		SHA    = (info[1] & ((int)1 << 29)) != 0;
	}

	printf("\t- %s\n", MMX   ? "MMX   is supported." : "MMX   is not supported.");
	printf("\t- %s\n", SSE   ? "SSE   is supported." : "SSE   is not supported.");
	printf("\t- %s\n", SSE2  ? "SSE2  is supported." : "SSE2  is not supported.");
	printf("\t- %s\n", SSE3  ? "SSE3  is supported." : "SSE3  is not supported.");
	printf("\t- %s\n", SSE41 ? "SSE41 is supported." : "SSE41 is not supported.");
	printf("\t- %s\n", SSE42 ? "SSE42 is supported." : "SSE42 is not supported.");
	printf("\t- %s\n", AES   ? "AES   is supported." : "AES   is not supported.");
	printf("\t- %s\n", SHA   ? "SHA   is supported." : "SHA   is not supported.");
	printf("\t- %s\n", AVX   ? "AVX   is supported." : "AVX   is not supported.");
	printf("\t- %s\n", AVX2  ? "AVX2  is supported." : "AVX2  is not supported.");
    
    return 0;
}
