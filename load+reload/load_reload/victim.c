#define _GNU_SOURCE
#include <stdio.h>
#include <stdint.h>
#include <string.h>
#include <sys/mman.h>

#include "util.h"

int main(void) {
	set_affinity(9);

	uint64_t *data = map_file((void *) 0x80000);
	char *secret = "super secret string";

	for (;;) {
		for (uint64_t i = 0; i < strlen(secret); i++) {
			for (uint64_t j = 0; j < 1e9; j++) {
				read_byte(&data[secret[i] * 4096]);
			}
			printf("hey\n");
		}
	}
}
