#ifndef ELEGRAM_THREAD_UTILS_H
#define ELEGRAM_THREAD_UTILS_H

#include <unistd.h>

void cleanup_close(int* arg) {
  close(*arg);
}

void cleanup_mutex_unlock(void* mutex) {
  pthread_mutex_unlock((pthread_mutex_t*) mutex);
}

#endif  // ELEGRAM_THREAD_UTILS_H
