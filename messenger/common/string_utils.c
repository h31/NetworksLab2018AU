#include "string_utils.h"

ssize_t safe_strlen(const char* str, size_t buf_size) {
  for (size_t i = 0; i < buf_size; ++i) {
    if (str[i] == '\0') {
      return i;
    }
  }
  return -1;
}
