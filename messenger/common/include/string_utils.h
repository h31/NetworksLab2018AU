#ifndef PROJECT_STRING_UTILS_H
#define PROJECT_STRING_UTILS_H

#include <stdlib.h>
#include <stdbool.h>

#include "ssize_t.h"

/**
 * Returns -1 if the string is not null terminated.
 * Returns the length of the string otherwise
 */
ssize_t safe_strlen(const char* str, size_t buf_size);

#endif  // PROJECT_STRING_UTILS_H
