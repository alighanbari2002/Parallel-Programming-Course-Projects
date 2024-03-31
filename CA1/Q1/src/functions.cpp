#include <cstdio>
#include <cstdlib>
#include <cstring>
#include <sched.h>
#include <unistd.h>

#include "functions.hpp"
#include "utils.hpp"

void print_SIMD_support_status(int (&info)[4])
{
    cpuid(info, 0x00000001);

    bool MMX   = (info[3] & ((int)1 << 23)) != 0;
    bool SSE   = (info[3] & ((int)1 << 25)) != 0;
    bool SSE2  = (info[3] & ((int)1 << 26)) != 0;
    bool SSE3  = (info[2] & ((int)1 << 0))  != 0;
    bool AES   = (info[2] & ((int)1 << 25)) != 0;
    bool SSE41 = (info[2] & ((int)1 << 19)) != 0;
    bool SSE42 = (info[2] & ((int)1 << 20)) != 0;
    bool AVX   = (info[2] & ((int)1 << 28)) != 0;

    cpuid(info, 0x80000000);
    bool AVX2  = false;
    bool SHA   = false;

    if (info[0] >= 0x00000007) {
        cpuid(info, 0x00000007);
        AVX2 = (info[1] & ((int)1 <<  5)) != 0;
        SHA  = (info[1] & ((int)1 << 29)) != 0;
    }

    printf("\t- MMX   is %s.\n", MMX   ? "supported" : "not supported");
    printf("\t- SSE   is %s.\n", SSE   ? "supported" : "not supported");
    printf("\t- SSE2  is %s.\n", SSE2  ? "supported" : "not supported");
    printf("\t- SSE3  is %s.\n", SSE3  ? "supported" : "not supported");
    printf("\t- SSE41 is %s.\n", SSE41 ? "supported" : "not supported");
    printf("\t- SSE42 is %s.\n", SSE42 ? "supported" : "not supported");
    printf("\t- AES   is %s.\n", AES   ? "supported" : "not supported");
    printf("\t- AVX   is %s.\n", AVX   ? "supported" : "not supported");
    printf("\t- AVX2  is %s.\n", AVX2  ? "supported" : "not supported");
    printf("\t- SHA   is %s.\n", SHA   ? "supported" : "not supported");
}

void print_SIMD_support(int (&info)[4])
{
    printf("\nSIMD Architecture Support:\n");

    // Check for SIMD architecture support (MMX, SSE, SSE2, SSE3, etc.)
    print_SIMD_support_status(info);
}

void get_cache_info(int (&info)[4], int& cache_type, int& cache_size)
{
    cpuid(info, 0x4 | (1 << 5)); // Set ECX to 1 to get cache level 1 information
    cache_type = info[0] & 0x1F;
    cache_size = ((info[1] >> 22) & 0x3FF) * 1024; // Size in KB
}

void print_cache_info(int (&info)[4])
{
    printf("\nCache Info:\n");

    // Get cache information for cache level 1
    int cache_type, cache_size;
    get_cache_info(info, cache_type, cache_size);
    printf("\t- Type: %d\n", cache_type);
    printf("\t- L1 Cache Size: %d KB\n", cache_size);
}

bool is_hyperthreading_supported(int (&info)[4])
{
    cpuid(info, 0x1);
    return (info[2] >> 28) & 0x1;
}

unsigned int cpuid_supported()
{
    unsigned int max_input_value = 0;

    __asm__ volatile(
        "xor %%eax, %%eax;"
        "cpuid;"
        "mov %%eax, %0;" :
        "=r" (max_input_value));

    return max_input_value;
}

unsigned int genuine_intel()
{
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

unsigned int is_HWMT_supported()
{
    unsigned int reg_edx = 0;

    if ((cpuid_supported() >= 1) && genuine_intel())
    {
        __asm__ volatile(
            "mov $1, %%eax;"
            "cpuid;"
            "mov %%edx, %0;" :
            "=r" (reg_edx)
        );
    }

    return (reg_edx & utils::hardware_constants::HWD_MT_BIT);
}

unsigned int get_max_num_cores_per_package()
{
    unsigned int ncore_determin;

    if (!is_HWMT_supported())
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
        (ncore_determin & utils::hardware_constants::NUM_CORE_BITS) >> 26) + 1; 
}

unsigned int get_max_numLP_per_package()
{
    unsigned int reg_ebx = 0;

    if (!is_HWMT_supported())
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
        (reg_ebx & utils::hardware_constants::NUM_LOGICAL_BITS) >> 16
    );
}

unsigned find_mask_width(unsigned int count_item)
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

    return mask_width;

    // Other implementation
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

unsigned char get_initial_apic_ID()
{
    unsigned int reg_ebx = 0;

    __asm__ volatile(
        "mov $1, %%eax;"
        "cpuid;"
        "mov %%ebx, %0;" : 
        "=r" (reg_ebx)
    );

    return (unsigned char)(
        (reg_ebx & utils::hardware_constants::INITIAL_APIC_ID_BITS) >> 24
    );
}

unsigned char get_nzb_subID(unsigned char fullID, unsigned char max_subID_value, unsigned char shift_count)
{
    unsigned int mask_width;
    unsigned char subID, mask_bits;
    mask_width = find_mask_width((unsigned int)(max_subID_value));
    mask_bits = ((unsigned char)(0xff << (shift_count + mask_width))) ^
        ((unsigned char)(0xff << shift_count));
    subID = fullID & mask_bits;
    return subID;
}

int get_num_active_cores()
{
    // Implementation of this function was inspired by these references:
    // - https://www.prowaretech.com/archive/CPU_ID/Detecting_Multi-Core_Processors.htm
    // - http://software.intel.com/en-us/articles/detecting-multi-core-processor-topology-in-an-ia-32-platform
	// (Behrad Elmi & Ali Ghanbari)

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

    while (j < sysconf(_SC_NPROCESSORS_CONF))
    {
        CPU_ZERO(&cset);
        CPU_SET(j, &cset);
        if (sched_setaffinity(getpid(), sizeof(cset), &cset) != -1)
        {
            usleep(0); // Ensure this thread is on the affinitized CPU
            apic_ID = get_initial_apic_ID();
            // 
            // Store SMT_ID and coreID of each logical processor
            // Shift value for SMT_ID is 0
            // Shift value for coreID is the mask width for maximum logical processors per core
            //
            tbl_SMT_ID[j] = get_nzb_subID(apic_ID, maxLP_per_core, 0);
            tbl_coreID[j] = get_nzb_subID(apic_ID,
                                        ncpp,
                                        mask_width_mlppc);
            packageID_mask =
                    ((unsigned char) (0xff << mask_width_mnlp));
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
    for (processor_num = 1; processor_num < numLP_enabled; ++processor_num)
    {
        processor_mask <<= 1;
        for (i = 0; i < core_num; ++i)
        {
            //
            // We may be comparing bit-fields of logical processors
            // residing in different packages, the code below assumes
            // that the bit-masks are the same on all processors in the system
            //
            if ((tbl_packageID[processor_num] | tbl_coreID[processor_num]) == coreID_bucket[i])
            {
                pCore_processor_mask[i] |= processor_mask;
                break;
            }
        }
        if (i == core_num)
        {
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

int get_num_logical_cores(int (&info)[4])
{
    cpuid(info, 0x1);
    return (info[1] >> 16) & 0xFF;
}

int get_num_physical_cores(int (&info)[4])
{
    cpuid(info, 0x4);
    return ((info[0] >> 26) & 0x3F) + 1;
}

void print_processor_info(int (&info)[4])
{
    printf("\nProcessor Info:\n");

    // Get processor type
    int processor_type[12];
    cpuid(processor_type + 0x0, 0x80000002);
    cpuid(processor_type + 0x4, 0x80000003);
    cpuid(processor_type + 0x8, 0x80000004);
    printf("\t- Type: %s\n", reinterpret_cast<char*>(processor_type));

    // Get count of physical and logical cores
    printf("\t- Physical Cores: %d\n", get_num_physical_cores(info));
    printf("\t- Logical Cores: %d\n", get_num_logical_cores(info));
    printf("\t- Enabled/Active Cores: %d\n", get_num_active_cores());

    // Check if hyperthreading is supported
    printf("\t- Hyperthreading is %s.\n", is_hyperthreading_supported(info) ? "supported" : "not supported");
}
