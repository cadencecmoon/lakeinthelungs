#ifdef PLATFORM_UNIX
#include <amw/bedrock/log.h>
#include <amw/bedrock/process.h>
#include <amw/bedrock/threads.h>

#include <pthread.h>
#include <sys/types.h>
#include <sys/cdefs.h>

void thread_create(
    thread_t *thread, 
    void *(*procedure)(void *), 
    void *argument)
{
    pthread_attr_t attr;
    pthread_attr_init(&attr);

    if (pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE) != 0) {
        log_fatal("Creating a joinable thread attribute with 'pthread_attr_setdetachstate' failed.");
        debugtrap();
    }
    if (pthread_create((pthread_t *)thread, &attr, procedure, argument) != 0) {
        log_fatal("Creating a thread with 'pthread_create' failed.");
        debugtrap();
    }
    pthread_attr_destroy(&attr);
}

void thread_destroy(thread_t thread)
{
    assert_debug(!pthread_equal((pthread_t)thread, pthread_self()));

    if (pthread_cancel((pthread_t)thread) != 0) {
        log_error("Destroying a thread with 'pthread_cancel' failed.");
        return;
    }
    if (pthread_join((pthread_t)thread, NULL) != 0) {
        log_error("Destroying a thread with 'pthread_join' failed.");
        return;
    }
}

void thread_join(thread_t thread)
{
    assert_debug(!pthread_equal((pthread_t)thread, pthread_self()));

    if (pthread_join((pthread_t)thread, NULL) != 0) {
        log_error("Joining a thread with 'pthread_join' (no cancel) failed.");
        return;
    }
}

u32 thread_array_index(const thread_t *threads, u32 thread_count)
{
    pthread_t self = pthread_self();
    for (u32 i = 0; i < thread_count; i++) {
        if (pthread_equal((pthread_t)threads[i], self)) {
            return i;
        }
    }
    return 0;
}

thread_t thread_current(void)
{
    return (thread_t)pthread_self();
}

void thread_affinity(
    u8       *stack_memory, 
    thread_t *threads, 
    u32       thread_count, 
    u32       cpu_count,
    u32       begin_cpu_index)
{
    cpu_set_t *cpusets = (cpu_set_t *)stack_memory;

    u32 i = 0;
    u32 j = begin_cpu_index;
    while (i < thread_count && j < cpu_count) {
        CPU_ZERO(&cpusets[i]);
        CPU_SET(j, &cpusets[i]);

        if (pthread_setaffinity_np((pthread_t)threads[i], sizeof(cpu_set_t), &cpusets[i]) != 0) {
            log_error("pthread_setaffinity_np failed for CPU %u and thread %u.", j, i);
            return;
        }
        i++; j++;
    }
}

#endif /* PLATFORM_UNIX */
