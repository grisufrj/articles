#!/usr/bin/env python3

from subprocess import Popen, PIPE

SAMPLES = 10

for main in range(16):
    # averages = []
    for counting in range(16):
        time_sum = 0

        for i in range(SAMPLES):
            p = Popen(["./core_measurement.out", str(main), str(counting)], stdout=PIPE)
            out, _ = p.communicate()
            time_sum += int(out.decode("utf-8"))

        average = int(time_sum / SAMPLES)
        cross = ((main % 8) // 4) != ((counting % 8) // 4)
        print(f"{main}x{counting} {average} {cross}")
        # averages.append(average)
