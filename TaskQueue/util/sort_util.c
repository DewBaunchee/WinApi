//
// Created by vmega on 18.07.2021.
//

#include "sort_util.h"
#include <stdio.h>
#include <string.h>

void sort_log_string(const char *sender, const char *action, char **current_state, unsigned int len) {
    if(is_logging) {
        if (len == 0)
            return;

        printf("[%s] - %s: { \n\"%s\"", sender, action, current_state[0]);
        for (int i = 1; i < len; i++) {
            printf(",\n \"%s\"", current_state[i]);
        }
        printf(" \n}\n");
    }
}

int compare(const char *first, const char *second) {
    unsigned int firstLength = strlen(first);
    unsigned int secondLength = strlen(second);
    if (firstLength > secondLength) return 1;
    if (firstLength < secondLength) return -1;

    for (int i = 0; i < firstLength; i++) {
        if (first[i] > second[i]) return 1;
        if (first[i] < second[i]) return -1;
    }

    return 0;
}