//
// Created by vmega on 16.10.2021.
//

#ifndef TASKQUEUE_STOPWATCH_H
#define TASKQUEUE_STOPWATCH_H

#include <sys/timeb.h>

typedef struct SStopWatch {
    char running;
    struct timeb start, end;
}* StopWatch;

StopWatch create_stopwatch();
void start(StopWatch);
void stop(StopWatch);
int elapsed(StopWatch stopWatch);
void free_stopwatch(StopWatch);

#endif //TASKQUEUE_STOPWATCH_H
