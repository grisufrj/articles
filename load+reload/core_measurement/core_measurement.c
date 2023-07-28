#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <stdint.h>
#include "util.h"

uint32_t MAIN_CORE;
uint32_t COUNTING_CORE;

uint64_t volatile count = 0;

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

int main(int argc, char **argv) {
	set_affinity(MAIN_CORE);

	if (argc < 3) {
		fprintf(stderr, "Usage: core_measurements MAIN_CORE COUNTING_CORE\n");
		return EXIT_FAILURE;
	}

	MAIN_CORE = strtol(argv[1], NULL, 0);
	COUNTING_CORE = strtol(argv[2], NULL, 0);

	uint64_t volatile delay;
	pthread_t thread;

	pthread_create(&thread, NULL, counting_thread, NULL);

	asm volatile (
		"loop%=:             \n\t"
		"movq (%%rbx), %%rax \n\t"
		"cmpq $100, %%rax    \n\t"
		"jae done%=          \n\t"
		"jmp loop%=          \n\t"
		"done%=:             \n\t"
		: "=a" (delay)
		: "b" (&count)
		:
	);

	printf("%lu\n", delay);
}
