#include <amwe/bedrock.h>

#if defined(LAKE_PLATFORM_UNIX)
#include <unistd.h>
#include <pthread.h>
#include <sys/types.h>
#include <sys/cdefs.h>

void bedrock_thread_create(
    bedrock_thread_t *thread, 
    void             *(*procedure)(void *), 
    void             *argument)
{
    pthread_attr_t attr;
    pthread_attr_init(&attr);

    if (pthread_attr_setdetachstate(&attr, PTHREAD_CREATE_JOINABLE) != 0) {
        bedrock_log_fatal("Creating a joinable thread attribute with 'pthread_attr_setdetachstate' failed.");
        lake_debugtrap();
    }
    if (pthread_create((pthread_t *)thread, &attr, procedure, argument) != 0) {
        bedrock_log_fatal("Creating a thread with 'pthread_create' failed.");
        lake_debugtrap();
    }
    pthread_attr_destroy(&attr);
}

void bedrock_thread_destroy(bedrock_thread_t thread)
{
    bedrock_assert_debug(!pthread_equal((pthread_t)thread, pthread_self()));

    if (pthread_cancel((pthread_t)thread) != 0) {
        bedrock_log_error("Destroying a thread with 'pthread_cancel' failed.");
        return;
    }
    if (pthread_join((pthread_t)thread, NULL) != 0) {
        bedrock_log_error("Destroying a thread with 'pthread_join' failed.");
        return;
    }
}

void bedrock_thread_join(bedrock_thread_t thread)
{
    bedrock_assert_debug(!pthread_equal((pthread_t)thread, pthread_self()));

    if (pthread_join((pthread_t)thread, NULL) != 0) {
        bedrock_log_error("Joining a thread with 'pthread_join' (no cancel) failed.");
        return;
    }
}

u32 bedrock_thread_array_index(
    const bedrock_thread_t *threads, 
    u32                     thread_count)
{
    pthread_t self = pthread_self();
    for (u32 i = 0; i < thread_count; i++) {
        if (pthread_equal((pthread_t)threads[i], self)) {
            return i;
        }
    }
    return 0;
}

bedrock_thread_t bedrock_thread_current(void)
{
    return (bedrock_thread_t)pthread_self();
}


void bedrock_thread_affinity(
    u8                 *stack_memory, 
    bedrock_thread_t   *threads, 
    u32                 thread_count, 
    u32                 cpu_count,
    u32                 begin_cpu_index)
{
    cpu_set_t *cpusets = (cpu_set_t *)stack_memory;

    u32 i = 0;
    u32 j = begin_cpu_index;
    while (i < thread_count && j < cpu_count) {
        CPU_ZERO(&cpusets[i]);
        CPU_SET(j, &cpusets[i]);

        if (pthread_setaffinity_np((pthread_t)threads[i], lake_sizeof(cpu_set_t), &cpusets[i]) != 0) {
            bedrock_log_error("pthread_setaffinity_np failed for CPU %u and thread %u.", j, i);
            return;
        }
        i++; j++;
    }
}
#endif /* LAKE_PLATFORM_UNIX */
