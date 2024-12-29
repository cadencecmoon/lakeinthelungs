#include <lake/bedrock/log.h>
#include <lake/riven.h>

#include <pthread.h>

#include <sys/types.h>
#include <sys/cdefs.h>

AMWAPI void AMWAPIENTRY 
thread_create(
        thread_t *thread, 
        void *(*procedure)(void *), 
        void *argument)
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

AMWAPI void AMWAPIENTRY 
thread_destroy(
        thread_t thread)
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

AMWAPI void AMWAPIENTRY 
thread_join(
        thread_t thread)
{
    assert_debug(!pthread_equal((pthread_t)thread, pthread_self()));

    if (pthread_join((pthread_t)thread, NULL) != 0) {
        log_error("pthread_join (no cancel) failed.");
        return;
    }
}

AMWAPI size_t AMWAPIENTRY 
thread_index(
        thread_t *threads, 
        size_t thread_count)
{
    if (!threads || thread_count <= 0)
        return 0;

    pthread_t self = pthread_self();
    for (;;) {
        for (size_t i = 0; i < thread_count; i++) {
            if (pthread_equal((pthread_t)threads[i], self)) {
                return i;
            }
        }
        log_error("thread_index() called from a thread not in the given worker pool.");
        return 0;
    }
}

AMWAPI thread_t AMWAPIENTRY 
thread_current(void)
{
    return (thread_t)pthread_self();
}

AMWAPI void AMWAPIENTRY
thread_affinity(
        uint8_t *stack_memory,
        thread_t *threads, 
        size_t thread_count, 
        size_t start_index)
{
    assert_debug(stack_memory && threads);
    assert_debug(thread_count > 0 && thread_count > start_index);

    cpu_set_t *cpusets = (cpu_set_t *)stack_memory;

    for (size_t i = start_index; i < thread_count; i++) {
        CPU_ZERO(&cpusets[i]);
        CPU_SET(i, &cpusets[i]);

        if (pthread_setaffinity_np(threads[i], sizeof(cpu_set_t), &cpusets[i]) != 0) {
            log_error("pthread_setaffinity_np failed for CPU %lu", i);
            return;
        }
    }
}
