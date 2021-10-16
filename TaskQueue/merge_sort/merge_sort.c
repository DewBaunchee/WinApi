//
// Created by vmega on 18.07.2021.
//

#include "merge_sort.h"
#include <stdlib.h>

#define MERGE_SENDER "Merger"

char **rec_merge(char **rows, unsigned int len) {

    if (len == 1) {
        char **part = calloc(1, sizeof(char **));
        part[0] = rows[0];
        return part;
    }

    unsigned int leftLen = len / 2;
    unsigned int rightLen = len - leftLen;
    char **left = rec_merge(rows, leftLen);
    char **right = rec_merge(rows + leftLen, len - leftLen);

    if (left[0] > right[0]) {
        char **temp = left;
        left = right;
        right = temp;

        unsigned int tempLen = leftLen;
        leftLen = rightLen;
        rightLen = tempLen;
    }

    sort_log_string(MERGE_SENDER, "Merging...", 0, 0);
    sort_log_string(MERGE_SENDER, "Left part", left, leftLen);
    sort_log_string(MERGE_SENDER, "Right part", right, rightLen);

    char **merged = calloc(leftLen + rightLen, sizeof(char *));
    int leftIndex = 0;
    int rightIndex = 0;
    for (int i = 0; i < len; i++) {
        if (leftIndex != leftLen && (rightIndex == rightLen || compare(left[leftIndex], right[rightIndex]) < 0)) {
            merged[i] = left[leftIndex++];
        } else {
            merged[i] = right[rightIndex++];
        }
    }

    free(left);
    free(right);
    sort_log_string(MERGE_SENDER, "Merged", merged, len);
    return merged;
}

void merge_sort(char **rows, unsigned int len) {
    sort_log_string(MERGE_SENDER, "Beginning", rows, len);
    char **sorted = rec_merge(rows, len);
    for (int i = 0; i < len; i++) rows[i] = sorted[i];
    free(sorted);
    sort_log_string(MERGE_SENDER, "Sorted.", rows, len);
}