//
// Created by vmega on 12.10.2021.
//

#include "task_queue.h"
#include <stdlib.h>
#include <stdio.h>

TaskPool create_task_pool(int maxSemaphoreValue) {
    TaskPool taskPool = calloc(1, sizeof(struct STaskPool));
    taskPool->mutex = CreateMutex(NULL, FALSE, NULL);
    taskPool->semaphore = CreateSemaphore(NULL, 0, maxSemaphoreValue, NULL);
    return taskPool;
}

void add_next_task(TaskPool taskPool, Task task) {
    WaitForSingleObject(taskPool->mutex, INFINITE);
    Task currentTask = taskPool->currentTask;
    if (!currentTask) {
        taskPool->currentTask = task;
    } else {
        while (currentTask->next)
            currentTask = currentTask->next;
        currentTask->next = task;
    }
    ReleaseMutex(taskPool->mutex);
    ReleaseSemaphore(taskPool->semaphore, 1, NULL);
}

Task next_task(TaskPool taskPool) {
    WaitForSingleObject(taskPool->semaphore, INFINITE);
    WaitForSingleObject(taskPool->mutex, INFINITE);
    Task returnableTask = taskPool->currentTask;
    if (returnableTask) {
        taskPool->currentTask = returnableTask->next;
    }
    ReleaseMutex(taskPool->mutex);
    return returnableTask;
}

void complete_pool(TaskPool taskPool) {
    while(ReleaseSemaphore(taskPool->semaphore, 10, NULL));
}

void free_task_pool(TaskPool taskPool) {
    Task currentTask = taskPool->currentTask;
    while (currentTask) {
        Task temp = currentTask;
        currentTask = currentTask->next;
        free(temp);
    }
    free(taskPool);
}