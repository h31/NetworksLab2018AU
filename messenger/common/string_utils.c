#include <string_utils.h>

bool check_strlen(const char* str, size_t len) {
  for (size_t i = 0; i < len; ++i) {
    if (str[i] == '\0') {
      return true;
    }
  }
  return false;
}
