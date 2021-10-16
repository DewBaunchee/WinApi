//
// Created by vmega on 16.10.2021.
//

#include <stdlib.h>
#include "stopwatch.h"

StopWatch create_stopwatch() {
    return calloc(1, sizeof(struct SStopWatch));
}

void start(StopWatch stopWatch) {
    if (!stopWatch->running) {
        ftime(&stopWatch->start);
        stopWatch->running = 1;
    }
}

void stop(StopWatch stopWatch) {
    if (stopWatch->running) {
        ftime(&stopWatch->end);
        stopWatch->running = 0;
    }
}

int elapsed(StopWatch stopWatch) {
    return (int) (1000.0 * (stopWatch->end.time - stopWatch->start.time)
                  + (stopWatch->end.millitm - stopWatch->start.millitm));
}

void free_stopwatch(StopWatch stopWatch) {
    free(stopWatch);
}