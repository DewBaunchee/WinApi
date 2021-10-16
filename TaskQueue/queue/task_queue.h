#ifndef TASKQUEUE_TASK_QUEUE_H
#define TASKQUEUE_TASK_QUEUE_H

#include <windows.h>

typedef struct SFilePart {
    unsigned int ordinal;
    unsigned int count;
    char** rows;
} FilePart;

typedef struct STask {
    void *data;

    struct STask * next;
} *Task;

typedef struct STaskPool {
    Task currentTask;
    HANDLE mutex;
    HANDLE semaphore;
} *TaskPool;

TaskPool create_task_pool(int maxSemaphoreValue);
void add_next_task(TaskPool taskPool, Task task);
Task next_task(TaskPool taskPool);
void complete_pool(TaskPool taskPool);
void free_task_pool(TaskPool taskPool);

#endif //TASKQUEUE_TASK_QUEUE_H
