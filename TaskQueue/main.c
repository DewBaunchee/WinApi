#include <stdio.h>
#include <libgen.h>
#include <stdlib.h>
#include <string.h>
#include <windows.h>
#include "queue/task_queue.h"
#include "merge_sort/merge_sort.h"
#include "stopwatch/stopwatch.h"

//#define ECHO
#define STOPWATCH
#define BUFFER_SIZE 40 * 1000 * 1024

HANDLE *create_threads(int count, TaskPool taskPool, TaskPool completedTasks);

void sort_file(FILE *in, FILE *out, int maxDegreeOfParallelism);

void slice_file(FILE *file, TaskPool taskPool, int partCount);

int count_rows(FILE *file);

void glue_file(FILE *file, TaskPool taskPool, int taskCount);

void free_tasks(Task *tasks, FilePart **parts, int count);

typedef struct SPoolPair {
    TaskPool taskPool;
    TaskPool completedTasks;
} *PoolPair;

int main(int argc, char *argv[]) {
    if (argc < 4) {
        printf("Not enough arguments: %s <input_file> <output_file> <thread_count>", basename(argv[0]));
        return -1;
    }

    int threadCount = atoi(argv[3]);
    if (threadCount < 1) {
        printf("[Primary thread] Thread count must be bigger than 0.");
        return -1;
    }

    sort_file(fopen(argv[1], "r"), fopen(argv[2], "w"), threadCount);
    return 0;
}

void sort_rows(FilePart *filePart) {
    for (int i = 0; i < filePart->count; i++) {
        for (int j = i; j < filePart->count; j++) {
            if (i != j && compare(filePart->rows[i], filePart->rows[j]) > 0) {
                char *temp = filePart->rows[i];
                filePart->rows[i] = filePart->rows[j];
                filePart->rows[j] = temp;
            }
        }
    }
}

DWORD WINAPI wait_for_task(void *data) {
    TaskPool taskPool = ((PoolPair) data)->taskPool;
    TaskPool completedTasks = ((PoolPair) data)->completedTasks;

    while (TRUE) {
        Task task = next_task(taskPool);
        if (task == NULL) {
            printf("[Thread %07ld] End of pool has been detected, breaking wait function...\n", GetCurrentThreadId());
            break;
        }

        FilePart *filePart = (FilePart *) task->data;
        printf("[Thread %07ld] Got file part with ordinal %d\n", GetCurrentThreadId(), filePart->ordinal);
        sort_rows(filePart);
        printf("[Thread %07ld] Putting sorted part with ordinal %d\n", GetCurrentThreadId(), filePart->ordinal);
        add_next_task(completedTasks, task);
    }
}

HANDLE *create_threads(int count, TaskPool taskPool, TaskPool completedTasks) {
    PoolPair poolPair = calloc(1, sizeof(struct SPoolPair));
    poolPair->taskPool = taskPool;
    poolPair->completedTasks = completedTasks;

    HANDLE *threads = calloc(count, sizeof(HANDLE));
    for (int i = 0; i < count; i++) {
        threads[i] = CreateThread(0,
                                  0,
                                  wait_for_task,
                                  poolPair,
                                  0,
                                  0);
        if (!threads[i]) {
            printf("[Primary thread] Cannot create thread: %lu\n", GetLastError());
            exit(-1);
        }
    }
    return threads;
}

void sort_file(FILE *in, FILE *out, int maxDegreeOfParallelism) {
    if (in == NULL) {
        printf("[Primary thread] Unable to read input file.\n");
        exit(-1);
    }
    if (out == NULL) {
        printf("[Primary thread] Unable to write output file.\n");
        exit(-1);
    }
    TaskPool taskPool = create_task_pool(maxDegreeOfParallelism);
    TaskPool completedTasks = create_task_pool(maxDegreeOfParallelism);
    StopWatch stopWatch = create_stopwatch();
    HANDLE *threads = create_threads(maxDegreeOfParallelism, taskPool, completedTasks);

    int slicingTime;
    int gluingTime;

    start(stopWatch);
    slice_file(in, taskPool, maxDegreeOfParallelism);
    stop(stopWatch);
    slicingTime = elapsed(stopWatch);

    start(stopWatch);
    glue_file(out, completedTasks, maxDegreeOfParallelism);
    stop(stopWatch);
    gluingTime = elapsed(stopWatch);

    complete_pool(taskPool);

    WaitForMultipleObjects(maxDegreeOfParallelism, threads, TRUE, INFINITE);

#ifdef STOPWATCH
    printf("\n[Primary thread] Slicing file elapsed %d ms, gluing file elapsed %d ms.\n", slicingTime, gluingTime);
#endif

    free(threads);
    free_stopwatch(stopWatch);
    free_task_pool(taskPool);
    free_task_pool(completedTasks);
}

void glue_file(FILE *file, TaskPool taskPool, int taskCount) {
    Task tasks[taskCount];
    FilePart *fileParts[taskCount];
    unsigned int rowCount = 0;
    for (int i = 0; i < taskCount; i++) {
        Task task = next_task(taskPool);
        if (task == NULL) {
            printf("[Primary thread] Null task is occurred, exiting...");
            exit(-1);
        }
        tasks[i] = task;
        FilePart *filePart = (FilePart *) task->data;
        printf("[Primary thread] Got part for gluing number %d...\n", filePart->ordinal);
        fileParts[filePart->ordinal] = filePart;
        rowCount += filePart->count;
    }

    char **rows = calloc(rowCount, sizeof(char *));
    int rowIndex = 0;
    for(int i = 0; i < taskCount; i++) {
        for(int j = 0; j < fileParts[i]->count; j++) {
            rows[rowIndex++] = fileParts[i]->rows[j];
        }
    }
    is_logging = 0;
    merge_sort(rows, rowCount);

#ifdef ECHO
    printf("\nSorted file:\n");
#endif
    for (int i = 0; i < rowCount; i++) {
#ifdef ECHO
        printf("%s\n", rows[i]);
#endif
        fprintf(file, "%s\n", rows[i]);
    }

    free(rows);
    free_tasks(tasks, fileParts, taskCount);
    fclose(file);
}

void free_tasks(Task *tasks, FilePart **parts, int count) {
    for (int i = 0; i < count; i++) {
        for (int j = 0; j < parts[i]->count; j++) {
            free(parts[i]->rows[j]);
        }
        free(parts[i]->rows);
        free(parts[i]);
        free(tasks[i]);
    }
}

void slice_file(FILE *file, TaskPool taskPool, int partCount) {
    int rowCount = count_rows(file);
    fseek(file, 0, SEEK_SET);
    int rowsPerPart = rowCount / partCount + 1;

#ifdef ECHO
    printf("Plain file:\n");
#endif
    char *buffer = calloc(BUFFER_SIZE, sizeof(char));
    for (int i = 0; i < partCount; i++) {
        FilePart *filePart = calloc(1, sizeof(FilePart));
        filePart->ordinal = i;
        filePart->rows = calloc(rowsPerPart, sizeof(char *));

        int j = 0;
        int currentRowCount = 0;
        while (currentRowCount < rowsPerPart) {
            char c = (char) fgetc(file);
            if (c == EOF || c == '\n') {
                filePart->rows[currentRowCount] = calloc(j + 1, sizeof(char));
                buffer[j] = 0;
                strcpy(filePart->rows[currentRowCount], buffer);
                currentRowCount++;
                j = 0;

#ifdef ECHO
                printf("%s\n", buffer);
#endif
                if (c == EOF) break;
            } else {
                buffer[j++] = c;
            }
        }

        filePart->count = currentRowCount;
        Task task = calloc(1, sizeof(struct STask));
        task->data = filePart;
        printf("[Primary thread] Putting part with ordinal: %d...\n", filePart->ordinal);
        add_next_task(taskPool, task);
    }
    free(buffer);
    fclose(file);
}

int count_rows(FILE *file) {
    int count = 0;
    char c;
    while ((c = (char) fgetc(file)) != EOF)
        if (c == '\n')
            count++;
    return count + 1;
}
