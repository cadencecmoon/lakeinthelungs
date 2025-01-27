#ifndef _GNU_SOURCE
#define _GNU_SOURCE
#endif

#include "../../bedrock.h"

#include <pthread.h>
#include <sys/types.h>
#include <sys/cdefs.h>

void bedrock_thread_create(thread_t *thread, void *(*procedure)(void *), void *argument)
{
    pthread_attr_t attr;
    pthread_attr_init(&attr);

    assert_debug(thread && procedure);

    if (pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE) != 0) {
        log_error("pthread_attr_setdetachstate failed.");
        debugtrap();
    }
    if (pthread_create((pthread_t *)thread, &attr, procedure, argument) != 0) {
        log_error("pthread_create failed.");
        debugtrap();
    }
    pthread_attr_destroy(&attr);
}

void bedrock_thread_destroy(thread_t thread)
{
    assert_debug(!pthread_equal((pthread_t)thread, pthread_self()));

    if (pthread_cancel((pthread_t)thread) != 0) {
        log_error("pthread_cancel failed.");
        return;
    }
    if (pthread_join((pthread_t)thread, NULL) != 0) {
        log_error("pthread_join failed.");
        return;
    }
}

void bedrock_thread_join(thread_t thread)
{
    assert_debug(!pthread_equal((pthread_t)thread, pthread_self()));

    if (pthread_join((pthread_t)thread, NULL) != 0) {
        log_error("pthread_join (no cancel) failed.");
        return;
    }
}

ssize bedrock_thread_index(thread_t *threads, ssize thread_count)
{
    if (!threads || thread_count <= 0)
        return 0;

    pthread_t self = pthread_self();
    for (;;) {
        for (ssize i = 0; i < thread_count; i++) {
            if (pthread_equal((pthread_t)threads[i], self)) {
                return i;
            }
        }
        log_error("thread_index() called from a thread not in the given worker pool.");
        return 0;
    }
}

thread_t bedrock_thread_current(void)
{
    return (thread_t)pthread_self();
}

void bedrock_thread_affinity(u8 *temp_stack_memory, thread_t *threads, ssize thread_count, ssize begin_cpu_index)
{
    assert_debug(temp_stack_memory && threads);

    cpu_set_t *cpusets = (cpu_set_t *)temp_stack_memory;
    ssize cpu_count = 0;

    bedrock_cpuinfo(&cpu_count, NULL, NULL);

    ssize i = 0; 
    ssize j = begin_cpu_index;
    while (i < thread_count && j < cpu_count) {
        CPU_ZERO(&cpusets[i]);
        CPU_SET(j, &cpusets[i]);

        if (pthread_setaffinity_np(threads[i], sizeof(cpu_set_t), &cpusets[i]) != 0) {
            log_error("pthread_setaffinity_np failed for CPU %lu and system thread %lu", j, i);
            return;
        }
        i++; j++;
    }
}
