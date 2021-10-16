//
// Created by vmega on 18.07.2021.
//

#ifndef SORTS_SORT_UTIL_H
#define SORTS_SORT_UTIL_H

int is_logging;

int compare(const char *first, const char *second);
void sort_log_string(const char *sender, const char *action, char **current_state, unsigned int len);

#endif //SORTS_SORT_UTIL_H
