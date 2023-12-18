#include <stdio.h>
#include <sched.h>
#include <unistd.h>
#include <string.h>
#ifdef 		_WIN32
#include <intrin.h>
#else
#include <x86intrin.h>
#endif

#ifdef 		_WIN32					//  Windows
#define 	cpuid    __cpuid
#else								//  Linux
void cpuid(int cpu_info[4], int info_type) {
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

#define INITIAL_APIC_ID_BITS 0xff000000
#define HWD_MT_BIT (1 << 28)
#define NUM_CORE_BITS 0xfc000000
#define NUM_LOGICAL_BITS 0xFF0000

typedef unsigned long int DWORD;

unsigned int cpuid_supported(void) {
    unsigned int max_input_value = 0;
    __asm__ volatile(
        "xor %%eax, %%eax;"
        "cpuid;"
        "mov %%eax, %0;" :
        "=r" (max_input_value));
    return max_input_value;
}

unsigned int genuine_intel(void) {
    unsigned int vendor_ID[3] = {0, 0, 0};
    __asm__ volatile(
        "xor %%eax, %%eax;"
        "cpuid;"
        "mov %%ebx, %0;" :
        "=r" (vendor_ID[0])
    );
    __asm__ volatile(
        "mov %%edx, %0;" :
        "=r" (vendor_ID[1])
    );
    __asm__ volatile(
        "mov %%ecx, %0;" :
        "=r" (vendor_ID[2])
    );
    return ((vendor_ID[0] == 'uneG') &&
            (vendor_ID[1] == 'Ieni') &&
            (vendor_ID[2] == 'letn'));
}

unsigned int is_HWMT_supported(void) {
    unsigned int reg_edx = 0;
    if((cpuid_supported() >= 1) && genuine_intel()) {
        __asm__ volatile(
            "mov $1, %%eax;"
            "cpuid;"
            "mov %%edx, %0;" :
            "=r" (reg_edx)
        );
    }
    return (reg_edx & HWD_MT_BIT);
}

unsigned int get_max_num_cores_per_package(void) {
    unsigned int ncore_determin;
    if(!is_HWMT_supported()) {
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

unsigned int get_max_numLP_per_package(void) {
    unsigned int reg_ebx = 0;
    if(!is_HWMT_supported()) {
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

unsigned find_mask_width(unsigned int count_item) {
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
    return mask_width;

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
}

unsigned char get_initial_apic_ID(void) {
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

unsigned char get_nzb_subID(unsigned char fullID, unsigned char max_subID_value, unsigned char shift_count) {
    unsigned int mask_width;
    unsigned char subID, mask_bits;
    mask_width = find_mask_width((unsigned int)(max_subID_value));
    mask_bits = ((unsigned char)(0xff << (shift_count + mask_width))) ^
        ((unsigned char)(0xff << shift_count));
    subID = fullID & mask_bits;
    return subID;
}

int get_num_active_cores() {
    // implementation of this function
    // was inspired by these references:
    // https://www.prowaretech.com/archive/CPU_ID/Detecting_Multi-Core_Processors.htm
    // http://software.intel.com/en-us/articles/detecting-multi-core-processor-topology-in-an-ia-32-platform
	// - Behrad Elmi & Ali Ghanbari

    cpu_set_t cset;
    unsigned int maxLP_per_core = 0;
    unsigned char apic_ID;
    unsigned char packageID_mask;
    unsigned char tbl_packageID[256];
    unsigned char tbl_coreID[256];
    unsigned char tbl_SMT_ID[256];
    memset(tbl_packageID, 0, 256 * sizeof(tbl_packageID[0]));
    memset(tbl_coreID, 0, 256 * sizeof(tbl_coreID[0]));
    memset(tbl_SMT_ID, 0, 256 * sizeof(tbl_SMT_ID[0]));
    CPU_ZERO(&cset);
    unsigned int mnlp = get_max_numLP_per_package();
    unsigned int ncpp = get_max_num_cores_per_package();
    maxLP_per_core = mnlp / ncpp;
    unsigned int mask_width_mlppc = find_mask_width(maxLP_per_core);
    unsigned int mask_width_mnlp = find_mask_width(mnlp);
    sched_getaffinity(getpid(), sizeof(cset), &cset);
    int j = 0, numLP_enabled = 0;

    while(j < sysconf(_SC_NPROCESSORS_CONF)) {
        CPU_ZERO(&cset);
        CPU_SET(j, &cset);
        if(sched_setaffinity(getpid(), sizeof(cset), &cset) != -1) {
            usleep(0); // Ensure this thread is on the affinitized CPU
            apic_ID = get_initial_apic_ID();
            // 
            // store SMT_ID and Core_ID of each logical processor
            // Shift value for SMT_ID is 0
            // Shift value for Core_ID is the mask width for maximum
            // logical processors per core
            //
            tbl_SMT_ID[j] = get_nzb_subID(apic_ID, maxLP_per_core, 0);
            tbl_coreID[j] = get_nzb_subID(apic_ID,
                                        ncpp,
                                        mask_width_mlppc);
            packageID_mask =
                    ((unsigned char) (0xff <<
                    mask_width_mnlp));
            tbl_packageID[j] = apic_ID & packageID_mask;
            numLP_enabled++;
        }
        j++;
    }

	DWORD pCore_processor_mask[256];
    unsigned char coreID_bucket [256];
    memset(pCore_processor_mask, 0, 256 * sizeof(pCore_processor_mask[0]));
    memset(coreID_bucket, 0, 256 * sizeof(coreID_bucket[0]));
    unsigned processor_mask;
    int processor_num;
    int i, core_num = 1;
    coreID_bucket[0] = tbl_packageID[0] | tbl_coreID[0];
    processor_mask = 1;
    pCore_processor_mask[0] = processor_mask;
    for(processor_num = 1; processor_num < numLP_enabled; ++processor_num) {
        processor_mask <<= 1;
        for(i = 0; i < core_num; ++i) {
            //
            // we may be comparing bit-fields of logical processors
            // residing in different packages, the code below assumes
            // that the bit-masks are the same on all processors in
            // the system
            //
            if((tbl_packageID[processor_num] | tbl_coreID[processor_num]) == coreID_bucket[i]) {
                pCore_processor_mask[i] |= processor_mask;
                break;
            }
        }
        if(i == core_num) {
            //
            // Did not match any bucket, start new bucket
            //
            coreID_bucket[i] = tbl_packageID[processor_num] |
            tbl_coreID[processor_num];
            pCore_processor_mask[i] = processor_mask;
            core_num++;
        }
    }
	return core_num;	
}

void print_group_info() {
    printf("Group Members:\n");
	printf("\t- Ali Ghanbari [810199473]\n");
	printf("\t- Behrad Elmi  [810199557]\n");
}

int main() {
    print_group_info();
    
	printf("\nProcessor Info:\n");

    // Get processor type
	int info[4];
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
	printf("\t- Enabled/Active Cores: %d\n", get_num_active_cores());

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
    printf("\t- L1 Cache Size: %d KB\n", cacheSize);

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
