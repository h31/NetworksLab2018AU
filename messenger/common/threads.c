#include "threads.h"

#include <stdlib.h>
#include <pthread.h>

struct thread {
	pthread_t pthread;
};

struct lock {
	pthread_mutex_t mutex;
};

thread_t* thread_create(void* (*entry_point)(void*), void* args) {
	thread_t *thread = malloc(sizeof(thread_t));
	pthread_create(&thread->pthread, NULL, entry_point, args);
	return thread;
}

void thread_join(thread_t *thread) {
	pthread_join(thread->pthread, NULL);
	free(thread);
}

void thread_interrupt(thread_t *thread) {
	pthread_cancel(thread->pthread);
}

void thread_is_interrupted() {
	pthread_testcancel();
}

lock_t* lock_create() {
	lock_t *lock = malloc(sizeof(lock_t));
	pthread_mutex_init(&lock->mutex, NULL);
	return lock;
}

void lock_destroy(lock_t *lock) {
	pthread_mutex_destroy(&lock->mutex);
	free(lock);
}

void lock(lock_t *lock) {
	pthread_mutex_lock(&lock->mutex);
}

void unlock(lock_t *lock) {
	pthread_mutex_unlock(&lock->mutex);
}
