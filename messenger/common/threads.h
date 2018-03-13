#ifndef THREADS_H_
#define THREADS_H_

typedef struct thread thread_t;
typedef struct lock lock_t;

thread_t* thread_create(void* (*entry_point)(void*), void* args);
void thread_join(thread_t *thread);
void thread_interrupt(thread_t *thread);
void thread_is_interrupted();

lock_t* lock_create();
void lock_destroy(lock_t *lock);
void lock(lock_t *lock);
void unlock(lock_t *lock);

#endif /* THREADS_H_ */
