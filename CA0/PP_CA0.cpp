#include <intrin.h>
#include <emmintrin.h>
#include <stdio.h>
#include <stdint.h>
#include <string.h>


#define cpuid __cpuid
#define cpuidex __cpuidex


typedef union {
	__m128i  m128iValue;

	uint8_t u8[16];     // 16 unsigned bytes
	int8_t i8[16];      // 16 signed bytes
	uint16_t u16[8];    // 8 unsigned words
	int16_t i16[8];     // 8 signed words
	uint32_t u32[4];    // 4 x 32-bit unsigned integers
	int32_t i32[4];     // 4 signed double words
	uint64_t u64[2];    // 2 x 64-bit unsigned integers
	int64_t i64[2];     // 2 signed quad words
} Register128;

void printRegister(const Register128* reg, const char* type) {
	if (strcmp(type, "u8") == 0) {
		// Print 16 unsigned bytes
		printf("Unsigned 8-bit values:\n");
		for (int i = 0; i < 16; ++i) {
			printf("%u ", reg->u8[i]);
		}
		printf("\n");
	}
	else if (strcmp(type, "i8") == 0) {
		// Print 16 signed bytes
		printf("Signed 8-bit values:\n");
		for (int i = 0; i < 16; ++i) {
			printf("%d ", reg->i8[i]);
		}
		printf("\n");

	}
	else if (strcmp(type, "u16") == 0) {
		// Print 8 unsigned words (16-bit)
		printf("Unsigned 16-bit values:\n");
		for (int i = 0; i < 8; ++i) {
			printf("%u ", reg->u16[i]);
		}
		printf("\n");

	}
	else if (strcmp(type, "i16") == 0) {
		// Print 8 signed words (16-bit)
		printf("Signed 16-bit values:\n");
		for (int i = 0; i < 8; ++i) {
			printf("%d ", reg->i16[i]);
		}
		printf("\n");

	}
	else if (strcmp(type, "u32") == 0) {
		// Print 4 unsigned double words (32-bit)
		printf("Unsigned 32-bit values:\n");
		for (int i = 0; i < 4; ++i) {
			printf("%u ", reg->u32[i]);
		}
		printf("\n");

	}
	else if (strcmp(type, "i32") == 0) {
		// Print 4 signed double words (32-bit)
		printf("Signed 32-bit values:\n");
		for (int i = 0; i < 4; ++i) {
			printf("%d ", reg->i32[i]);
		}
		printf("\n");

	}
	else if (strcmp(type, "u64") == 0) {
		// Print 2 unsigned quad words (64-bit)
		printf("Unsigned 64-bit values:\n");
		for (int i = 0; i < 2; ++i) {
			printf("%llu ", reg->u64[i]);
		}
		printf("\n");

	}
	else if (strcmp(type, "i64") == 0) {
		// Print 2 signed quad words (64-bit)
		printf("Signed 64-bit values:\n");
		for (int i = 0; i < 2; ++i) {
			printf("%lld ", reg->i64[i]);
		}
		printf("\n");

	}
	else {
		printf("Unknown type: %s\n", type);
	}
}


void printCacheAndTLBInfo() {
	int cpuInfo[4];
	int i, j;
	unsigned char descriptor;

	cpuid(cpuInfo, 2);
	unsigned int descriptors[12] = { cpuInfo[0], cpuInfo[1], cpuInfo[2], cpuInfo[3] };

	printf("Cache and TLB information:\n");
	for (i = 0; i < 4; ++i) {  // There are four registers: EAX, EBX, ECX, EDX
		for (j = 0; j < 4; ++j) {  // Each register contains 4 descriptor bytes
			descriptor = (descriptors[i] >> (j * 8)) & 0xFF;

			// Ignore descriptor 0x00 (invalid descriptor) and 0xFF (no more information)
			if (descriptor == 0x00 || descriptor == 0xFF) continue;

			// Output the descriptor
			printf("Descriptor: 0x%02X\n", descriptor);

			// Map descriptor to known cache or TLB configuration
			switch (descriptor) {
			case 0x01: printf("TLB: Instruction, 4K pages, 4-way set associative, 32 entries\n"); break;
			case 0x02: printf("TLB: Instruction, 4M pages, fully associative, 2 entries\n"); break;
			case 0x03: printf("TLB: Data, 4K pages, 4-way set associative, 64 entries\n"); break;
			case 0x06: printf("L1 Instruction Cache: 8 KB, 4-way set associative, 32-byte line size\n"); break;
			case 0x08: printf("L1 Instruction Cache: 16 KB, 4-way set associative, 32-byte line size\n"); break;
			case 0x0A: printf("L1 Data Cache: 8 KB, 2-way set associative, 32-byte line size\n"); break;
			case 0x0C: printf("L1 Data Cache: 16 KB, 4-way set associative, 32-byte line size\n"); break;
			case 0x22: printf("L3 Cache: 512 KB, 4-way set associative, 64-byte line size, 2 lines per sector\n"); break;
			case 0x45: printf("L2 Cache: 2 MB, 4-way set associative, 32-byte line size\n"); break;
			case 0x66: printf("L1 Data Cache: 8 KB, 4-way set associative, 64-byte line size\n"); break;
			case 0x68: printf("L1 Data Cache: 32 KB, 4-way set associative, 64-byte line size\n"); break;
			case 0x6C: printf("L1 Data Cache: 16 KB, 8-way set associative, 64-byte line size\n"); break;
			case 0x70: printf("Trace Cache: 12K-micro-op, 8-way set associative\n"); break;
			case 0x80: printf("L2 Cache: 512 KB, 8-way set associative, 64-byte line size\n"); break;
			case 0x82: printf("L2 Cache: 256 KB, 8-way set associative, 32-byte line size\n"); break;
			case 0x83: printf("L2 Cache: 512 KB, 8-way set associative, 32-byte line size\n"); break;
			case 0x84: printf("L2 Cache: 1 MB, 8-way set associative, 32-byte line size\n"); break;
			case 0x85: printf("L2 Cache: 2 MB, 8-way set associative, 32-byte line size\n"); break;
			default: printf("Unknown or vendor-specific descriptor: 0x%02X\n", descriptor); break;
			}
		}
	}
}

void print_Frequencies()
{
	int cpuInfo[4];
	__cpuid(cpuInfo, 0x16);

	int baseFreq = cpuInfo[0];
	int maxFreq = cpuInfo[1];
	int busFreq = cpuInfo[2];

	printf("CPU Base Frequency:%d MHz \n", baseFreq);
	printf("CPU Maximum Frequency:%d MHz \n", maxFreq);
	printf("CPU Bus Frequency:%d MHz \n", busFreq);
}

void getMaxAddressableLogicalCores() {
	int CPUInfo[4] = { -1 };
	cpuid(CPUInfo, 1);
	int logicalCores = (CPUInfo[1] >> 16) & 0xFF;

	printf("Max Addressable Logical Cores:  %d\n", logicalCores);

}

void getPhysicalCoreCount() {
	int CPUInfo[4] = { -1 };
	cpuidex(CPUInfo, 0x4, 0);
	int coresPerPackage = ((CPUInfo[0] >> 26) & 0x3F) + 1;
	printf("Number of physical cores: %d\n", coresPerPackage);
}

int main()
{
	// Get processor brand string
	char brand[0x40] = { 0 };
	cpuid((int*)(brand), 0x80000002);
	cpuid((int*)(brand + 16), 0x80000003);
	cpuid((int*)(brand + 32), 0x80000004);
	printf("Processor Brand: %s\n", brand);

	int cpuInfo[4];

	
	//Get processor frequency information
	int frequencyInfoex[4] , frequencyInfo[4];
	cpuid(frequencyInfo, 0x16);
	cpuidex(frequencyInfoex, 0x16,0);
	
	int baseFreq = frequencyInfo[0];      
	int maxFreq = frequencyInfo[1];       

	int baseFreqex = frequencyInfoex[0];     
	int maxFreqex = frequencyInfoex[1];       

	printf("Base Frequency(cpuidex): %d MHz\n", baseFreqex);
	printf("Base Frequency(cpuid): %d MHz\n\n\n", baseFreq);


	printf("Maximum Frequency(cpuid): %d MHz\n", maxFreq);
	printf("Maximum Frequency(cpuidex): %d MHz\n\n\n", maxFreqex);
	



	// Check for Hyperthreading support
	int info[4];
	cpuid(info, 0x00000001);
	int hyperthreading = (info[3] & (1 << 28)) != 0;
	printf("Hyperthreading %s\n", hyperthreading ? "Supported" : "NOT Supported");

	getPhysicalCoreCount();
	getMaxAddressableLogicalCores();




	// Check for Intel Turbo Boost Technology
	cpuid(info, 0x00000006);  // Leaf 0x06, Power and Thermal Management
	int turboBoost = (info[2] & (1 << 1)) != 0;  // Bit 1 of ECX indicates Turbo Boost support

	if (turboBoost) {
		printf("\n\n\n\nIntel Turbo Boost is Supported.\n\n\n");
	}
	else {
		printf("\n\n\n\nIntel Turbo Boost is NOT Supported.\n\n\n");
	}


	// Chech SIMD instructions support
	cpuid(info, 0x00000001);
	int MMX = (info[3] & ((int)1 << 23)) != 0;
	int SSE = (info[3] & ((int)1 << 25)) != 0;
	int SSE2 = (info[3] & ((int)1 << 26)) != 0;
	int SSE3 = (info[2] & ((int)1 << 0)) != 0;
	int AES = (info[2] & ((int)1 << 25)) != 0;
	int SSE41 = (info[2] & ((int)1 << 19)) != 0;
	int SSE42 = (info[2] & ((int)1 << 20)) != 0;
	int AVX = (info[2] & ((int)1 << 28)) != 0;

	printf("%s\n", MMX ? "MMX Supported" : "MMX NOT Supported");
	printf("%s\n", SSE ? "SSE Supported" : "SSE NOT Supported");
	printf("%s\n", SSE2 ? "SSE2 Supported" : "SSE2 NOT Supported");
	printf("%s\n", SSE3 ? "SSE3 Supported" : "SSE3 NOT Supported");
	printf("%s\n", SSE41 ? "SSE41 Supported" : "SSE41 NOT Supported");
	printf("%s\n", SSE42 ? "SSE42 Supported" : "SSE42 NOT Supported");
	printf("%s\n", AES ? "AES Supported" : "AES NOT Supported");
	printf("%s\n", AVX ? "AVX Supported" : "AVX NOT Supported");

	// TLB and Cache information

	//printCacheAndTLBInfo(); // usign leaf 2

	//int cpuInfo[4];
	// L1 Cache and TLB Information (CPUID leaf 0x80000005)
	cpuid(cpuInfo, 0x80000005);
	printf("\n\n\nL1 Cache and TLB Information:\n");
	printf("L1 Data Cache Size: %d KB\n", (cpuInfo[2] >> 24)); // ECX register, bits 31-24
	printf("L1 Instruction Cache Size: %d KB\n", (cpuInfo[3] >> 24)); // EDX register, bits 31-24

	// L2 Cache Information (CPUID leaf 0x80000006)
	cpuid(cpuInfo, 0x80000006);
	printf("L2 Cache Information:\n");
	printf("L2 Cache Size: %d KB\n", (cpuInfo[2] >> 16) & 0xFFFF); // ECX register, bits 31-16
	printf("L3 Cache Size: %d MB\n", (cpuInfo[3] >> 18)); // EDX register, bits 31-18


	// Union and type casting
	__m128i m128iReg = _mm_set_epi32(0x11223344, 0x55667788, 0x99AABBCC, 0xDDEEFF00);

	// Create the union and assign the __m128i value
	Register128 reg;
	reg.m128iValue = m128iReg;

	printRegister(&reg, "u8");
	printRegister(&reg, "i8");
	printRegister(&reg, "i32");
	printRegister(&reg, "u32");
	

	return 0;
}



