#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <sched.h>

void set_affinity(uint32_t core) {
	cpu_set_t cpuset;
	CPU_ZERO(&cpuset);
	CPU_SET(core, &cpuset);

	pthread_t current = pthread_self();
	pthread_setaffinity_np(current, sizeof(cpu_set_t), &cpuset);
}

void read_byte(uint64_t *ptr) {
	asm volatile (
			"movq (%%rcx), %%rcx \n\t"
			:
			: "c" (ptr)
			:
	);
}
