#define _GNU_SOURCE
#include <stdlib.h>
#include <stdio.h>
#include <pthread.h>
#include <sched.h>
#include <fcntl.h>
#include <sys/stat.h>

/* 1 MiB */
#define FILE_SIZE 1048576
#define INDEX_NUM 131072
#define DATA "./data"

void set_affinity(uint32_t core) {
	cpu_set_t cpuset;
	CPU_ZERO(&cpuset);
	CPU_SET(core, &cpuset);

	pthread_t current = pthread_self();
	pthread_setaffinity_np(current, sizeof(cpu_set_t), &cpuset);
}

void *map_file(void *addr) {
	int fd = open(DATA, O_RDONLY);
	if (fd == -1) {
		fprintf(stderr, "open error\n");
		exit(EXIT_FAILURE);
	}

	struct stat stat;
	fstat(fd, &stat);

	void *ret = mmap(addr, stat.st_size, PROT_READ, MAP_SHARED, fd, 0);
	if (ret == MAP_FAILED || ret != addr) {
		fprintf(stderr, "mmap error\n");
		exit(EXIT_FAILURE);
	}

	return ret;
}

void read_byte(uint64_t *ptr) {
	asm volatile (
			"movq (%%rcx), %%rcx \n\t"
			:
			: "c" (ptr)
			:
	);
}
