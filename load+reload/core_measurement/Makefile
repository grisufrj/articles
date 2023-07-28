CFLAGS = -Wall -Werror

all: core_measurement.out

core_measurement.out: core_measurement.c
	$(CC) $(CFLAGS) -g -o core_measurement.out core_measurement.c -lpthread

.PHONY: clean

clean:
	rm -f core_measurement.out
