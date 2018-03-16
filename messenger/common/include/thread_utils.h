#ifndef ELEGRAM_THREAD_UTILS_H
#define ELEGRAM_THREAD_UTILS_H

#include <unistd.h>

static inline void cleanup_close(int* arg) {
  close(*arg);
}

static inline void cleanup_mutex_unlock(void* mutex) {
  pthread_mutex_unlock((pthread_mutex_t*) mutex);
}

static inline void cleanup_rwlock_unlock(void* rwlock) {
  pthread_rwlock_unlock((pthread_rwlock_t*) rwlock);
}

#endif  // ELEGRAM_THREAD_UTILS_H
