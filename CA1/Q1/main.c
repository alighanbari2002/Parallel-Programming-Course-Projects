#define _GNU_SOURCE
#include <stdio.h>
#include <x86intrin.h>
#include <sched.h>
#include <unistd.h>
#include <string.h>

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

#define INITIAL_APIC_ID_BITS 0xff000000
#define HWD_MT_BIT (1 << 28)
#define NUM_CORE_BITS 0xfc000000
#define NUM_LOGICAL_BITS 0xFF0000

typedef unsigned long int DWORD;

unsigned int CpuIDSupported(void);

unsigned int GenuineIntel(void);

unsigned int is_HWMT_Supported(void);

unsigned int GetMaxNumCoresPerPackage(void);

unsigned int GetMaxNumLPperPackage(void);

unsigned find_maskwidth(unsigned int count_item);

unsigned char GetInitialApicId(void);

unsigned char GetNzbSubID(
    unsigned char Full_ID,
    unsigned char MaxSubIDvalue,
    unsigned char Shift_Count);

int GetNumActiveCores();

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

	printf("\t- Enabled/Active Cores: %d\n", GetNumActiveCores());

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
    unsigned short MMX   = 0;
	unsigned short SSE   = 0;
	unsigned short SSE2  = 0;
	unsigned short SSE3  = 0;
	unsigned short AES   = 0;
	unsigned short SSE41 = 0;
	unsigned short SSE42 = 0;
	unsigned short AVX   = 0;
	unsigned short AVX2  = 0;
	unsigned short SHA   = 0;

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

unsigned int CpuIDSupported(void)
{
    unsigned int MaxInputValue = 0;
    __asm__ volatile(
        "xor %%eax, %%eax;"
        "cpuid;"
        "mov %%eax, %0;" :
        "=r" (MaxInputValue));
    return MaxInputValue;
}

unsigned int GenuineIntel(void)
{
    unsigned int VendorID[3] = {0, 0, 0};
    __asm__ volatile(
        "xor %%eax, %%eax;"
        "cpuid;"
        "mov %%ebx, %0;" :
        "=r" (VendorID[0])
    );
    __asm__ volatile(
        "mov %%edx, %0;" :
        "=r" (VendorID[1])
    );
    __asm__ volatile(
        "mov %%ecx, %0;" :
        "=r" (VendorID[2])
    );
    return ((VendorID[0] == 'uneG') &&
            (VendorID[1] == 'Ieni') &&
            (VendorID[2] == 'letn'));
}

unsigned int is_HWMT_Supported(void)
{
    unsigned int Regedx = 0;
    if((CpuIDSupported() >= 1) && GenuineIntel())
    {
        __asm__ volatile(
            "mov $1, %%eax;"
            "cpuid;"
            "mov %%edx, %0;" :
            "=r" (Regedx)
        );
    }
    return (Regedx & HWD_MT_BIT);
}

unsigned int GetMaxNumCoresPerPackage(void)
{
    unsigned int ncore_determin;
    if(!is_HWMT_Supported())
    {
        return 1;
    }
    __asm__ volatile(
        "mov $4, %%eax;"
        "xor %%ecx, %%ecx;"
        "cpuid;"
        "mov %%eax, %0;" :
        "=r" (ncore_determin)
    );
    return (unsigned int)(
        (ncore_determin & NUM_CORE_BITS) >> 26) + 1; 
}

unsigned int GetMaxNumLPperPackage(void)
{
    unsigned int reg_ebx = 0;
    if(!is_HWMT_Supported())
    {
        return 0;
    }
    __asm__ volatile(
        "mov $1, %%eax;"
        "cpuid;"
        "mov %%ebx, %0" :
        "=r" (reg_ebx)
    );
    return (unsigned int)(
        (reg_ebx & NUM_LOGICAL_BITS) >> 16
    );
}

unsigned find_maskwidth(unsigned int count_item)
{
    unsigned int mask_width = 0, cnt = count_item;
    __asm__ volatile(
        "mov %1, %%eax\n\t"
        "mov $0, %%ecx\n\t"
        "mov %%ecx, %0\n\t"
        "dec %%eax\n\t"
        "bsr %%ax, %%cx\n\t"
        "jz next\n\t"
        "inc %%cx\n\t"
        "mov %%ecx, %0\n\t"
        "next: \n\t"
        "mov %0, %%eax;" :
        "=r" (mask_width) :
        "r" (mask_width), "r" (cnt)
    );

    // other implementation
    // __asm__ volatile(
    // #ifdef __x86_64__
    // "push %%rcx\n\t"
    // "push %%rax\n\t"
    // #else
    // "pushl %%ecx\n\t"
    // "pushl %%rax\n\t"
    // #endif
    // "xorl %%ecx, %%ecx" :
    // "=c" (mask_width) :
    // "a" (cnt)
    // );
    // __asm__ volatile(
    //     "decl %%eax\n\t"
    //     "bsrw %%ax, %%cx\n\t"
    //     "jz next\n\t"
    //     "incw %%cx\n\t" :
    //     "=c" (mask_width) :
    // );
    // __asm__ volatile(
    //     "next: \n\t"
    //     #ifdef __x86_64__
    //     "pop %rax\n\t"
    //     "pop %rcx\n\t"
    //     #else
    //     "popl %eax\n\t"
    //     "popl %ecx"
    //     #endif
    // );
    return mask_width;
}

unsigned char GetInitialApicId(void)
{
    unsigned int reg_ebx = 0;
    __asm__ volatile(
        "mov $1, %%eax;"
        "cpuid;"
        "mov %%ebx, %0;" : 
        "=r" (reg_ebx)
    );
    return (unsigned char)(
        (reg_ebx & INITIAL_APIC_ID_BITS) >> 24
    );
}

unsigned char GetNzbSubID(
    unsigned char Full_ID,
    unsigned char MaxSubIDvalue,
    unsigned char Shift_Count)
{
    unsigned int MaskWidth;
    unsigned char SubID, MaskBits;
    MaskWidth = find_maskwidth((unsigned int)(MaxSubIDvalue));
    MaskBits = ((unsigned char)(0xff << (Shift_Count + MaskWidth))) ^
        ((unsigned char)(0xff << Shift_Count));
    SubID = Full_ID & MaskBits;
    return SubID;
}

int GetNumActiveCores()
{
    // implementation of this function
    // was inspired by these references:
    // https://www.prowaretech.com/archive/CPU_ID/Detecting_Multi-Core_Processors.htm
    // http://software.intel.com/en-us/articles/detecting-multi-core-processor-topology-in-an-ia-32-platform
	// - Behrad Elmi & Ali Ghanbari

    cpu_set_t cset;
    int j, numLP_enabled = 0;
    unsigned int MaxLPPerCore = 0;
    unsigned char apicId;
    unsigned char PackageIDMask;
    unsigned char tblPkg_ID[256];
    unsigned char tblCore_ID[256];
    unsigned char tblSMT_ID[256];
    memset(tblPkg_ID, 0, 256 * sizeof(tblPkg_ID[0]));
    memset(tblCore_ID, 0, 256 * sizeof(tblCore_ID[0]));
    memset(tblSMT_ID, 0, 256 * sizeof(tblSMT_ID[0]));
    CPU_ZERO(&cset);
    unsigned int mnlp = GetMaxNumLPperPackage();
    unsigned int ncpp = GetMaxNumCoresPerPackage();
    MaxLPPerCore = mnlp / ncpp;
    unsigned int maskwidth_mlppc = find_maskwidth(MaxLPPerCore);
    unsigned int maskwidth_mnlp = find_maskwidth(mnlp);
    sched_getaffinity(getpid(), sizeof(cset), &cset);
    j = 0;

    while(j < sysconf(_SC_NPROCESSORS_CONF))
    {
        CPU_ZERO(&cset);
        CPU_SET(j, &cset);
        if(sched_setaffinity(getpid(), sizeof(cset), &cset) != -1)
        {
            usleep(0); // Ensure this thread is on the affinitized CPU
            apicId = GetInitialApicId();
            // 
            // store SMT_ID and Core_ID of each logical processor
            // Shift value for SMT_ID is 0
            // Shift value for Core_ID is the mask width for maximum
            // logical processors per core
            //
            tblSMT_ID[j] = GetNzbSubID(apicId, MaxLPPerCore, 0);
            tblCore_ID[j] = GetNzbSubID(apicId,
                                        ncpp,
                                        maskwidth_mlppc);
            PackageIDMask =
                    ((unsigned char) (0xff <<
                    maskwidth_mnlp));
            tblPkg_ID[j] = apicId & PackageIDMask;
            numLP_enabled++;
        }
        j++;
    }

	DWORD pCoreProcessorMask[256];
    unsigned char CoreIDBucket [256];
    memset(pCoreProcessorMask, 0, 256 * sizeof(pCoreProcessorMask[0]));
    memset(CoreIDBucket, 0, 256 * sizeof(CoreIDBucket[0]));
    unsigned ProcessorMask;
    int ProcessorNum;
    int i, CoreNum = 1;
    CoreIDBucket[0] = tblPkg_ID[0] | tblCore_ID[0];
    ProcessorMask = 1;
    pCoreProcessorMask[0] = ProcessorMask;
    for(ProcessorNum = 1; ProcessorNum < numLP_enabled; ProcessorNum++) 
    {
        ProcessorMask <<= 1;
        for(i = 0; i < CoreNum; i++) 
        {
            //
            // we may be comparing bit-fields of logical processors
            // residing in different packages, the code below assumes
            // that the bit-masks are the same on all processors in
            // the system
            //
            if((tblPkg_ID[ProcessorNum] | tblCore_ID[ProcessorNum]) == CoreIDBucket[i])
            {
                pCoreProcessorMask[i] |= ProcessorMask;
                break;
            }
        }
        if(i == CoreNum)
        {
            //
            // Did not match any bucket, start new bucket
            //
            CoreIDBucket[i] = tblPkg_ID[ProcessorNum] |
            tblCore_ID[ProcessorNum];
            pCoreProcessorMask[i] = ProcessorMask;
            CoreNum++;
        }
    }
	return CoreNum;	
}