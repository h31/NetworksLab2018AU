#ifndef ELEGRAM_THREAD_UTILS_H
#define ELEGRAM_THREAD_UTILS_H

#include "socket_utils.h"

static inline void cleanup_mutex_unlock(void* mutex) {
  pthread_mutex_unlock((pthread_mutex_t*) mutex);
}

static inline void cleanup_rwlock_unlock(void* rwlock) {
  pthread_rwlock_unlock((pthread_rwlock_t*) rwlock);
}

static inline void cleanup_close_socket(void* socket) {
  close_socket(*((socket_t*) socket));
}

#endif  // ELEGRAM_THREAD_UTILS_H
