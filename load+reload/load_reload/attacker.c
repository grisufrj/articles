#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include <unistd.h>
#include <sys/mman.h>
#include <fcntl.h>
#include "util.h"

#define MAIN_CORE 1
#define COUNTING_CORE 2

/* #define RDTSC */

uint64_t load_count(uint64_t *addr);

uint64_t volatile count = 0;

#ifndef RDTSC
void *counting_thread(void *args) {
	set_affinity(COUNTING_CORE);
	asm volatile (
		"xorq %%rax, %%rax   \n\t"
		"loop%=:             \n\t"
		"incq %%rax          \n\t"
		"movq %%rax, (%%rbx) \n\t"
		"jmp loop%=          \n\t"
		:
		: "b" (&count)
		: "rax"
	);

	pthread_exit(NULL);
}
#endif

#ifndef RDTSC
uint64_t load_count(uint64_t *addr) {
	uint64_t volatile time;
	asm volatile (
		"mfence              \n\t"
		"lfence              \n\t"
		"movq (%%rbx), %%rcx \n\t"
		"lfence              \n\t"
		"movq (%%rax), %%rdx \n\t"
		"lfence              \n\t"
		"mfence              \n\t"
		"movq (%%rbx), %%rax \n\t"
		"subq %%rcx, %%rax   \n\t"
		: "=a" (time)
		: "a" (addr), "b" (&count)
		: "rcx", "rdx"
	);
	return time;
}
#else
uint64_t load_count(uint64_t *addr) {
	uint64_t volatile time;
	asm volatile (
		"mfence              \n\t"
		"lfence              \n\t"
		"rdtsc               \n\t"
		"lfence              \n\t"
		"movl %%eax, %%ebx   \n\t"
		"movq (%%rcx), %%rcx \n\t"
		"lfence              \n\t"
		"rdtsc               \n\t"
		"subl %%ebx, %%eax   \n\t"
		: "=a" (time)
		: "c" (addr)
		: "rbx"
	);
	return time;
}
#endif

int main(void) {
	set_affinity(MAIN_CORE);

	uint64_t *data = map_file((void *) 0x50000);

#ifndef RDTSC
	pthread_t thread;
	pthread_create(&thread, NULL, counting_thread, NULL);
	while (count < 1e7)
		;
#endif

	for (;;) {
		uint8_t winner;
		double largest = 0;

		for (uint8_t byte = 0; byte < 0xff; byte++) {
			double sum;
			uint64_t samples;
			for (samples = sum = 0; samples < 1e5; samples++) {
				uint64_t time = load_count(&data[byte * 4096]);
				sum += time;
			}

			double average = sum / samples;
			if (average > largest) {
				winner = byte;
				largest = average;
			}
		}

		printf("%c\n", winner);
	}
}
