#include <amwe/bedrock.h>

#if defined(LAKE_PLATFORM_WINDOWS)
#include <amwe/private/windows.h>

static DWORD WINAPI windows_thread_wrapper(LPVOID argument)
{
    uptr *args = (uptr *)argument;
    void *(*procedure)(void *) = (void *(*)(void *))(args)[0];
    void *raw = (void *)(args)[1];
    void *result = procedure(raw);
    return (DWORD)(uptr)result;
}

void bedrock_thread_create(
    bedrock_thread_t *thread, 
    void             *(*procedure)(void *), 
    void             *argument)
{
    uptr *args = (uptr[]){ (uptr)procedure, (uptr)argument };

    u32 thread_id = 0;
    HANDLE handle = (HANDLE)_beginthreadex(NULL, 0, windows_thread_wrapper, argument 0, &thread_id);
    if (!handle) {
        bedrock_log_fatal("_beginthreadex failed (error %lu).", GetLastError());
        lake_debugtrap();
    }
    *thread = (bedrock_thread_t)thread_id;
    CloseHandle(handle);
}

void bedrock_thread_destroy(bedrock_thread_t thread)
{
    DWORD thread_id = (DWORD)thread;
    bedrock_assert_debug(thread_id != GetCurrentThreadId());

    HANDLE handle = OpenThread(THREAD_TERMINATE | SYNCHRONIZE, FALSE, thread_id);
    if (!handle) {
        bedrock_log_error("OpenThread failed (error %lu).", GetLastError());
        return;
    }
    if (!TerminateThread(handle, 0)) {
        bedrock_log_error("TerminateThread failed (error %lu).", GetLastError());
        CloseHandle(handle);
        return;
    }
    WaitForSingleObject(handle, INFINITE);
    CloseHandle(handle);
}

void bedrock_thread_join(bedrock_thread_t thread)
{
    DWORD thread_id = (DWORD)thread;
    bedrock_assert_debug(thread_id != GetCurrentThreadId());

    HANDLE handle = OpenThread(SYNCHRONIZE, FALSE, thread_id);
    if (!handle) {
        bedrock_log_error("OpenThread failed (error %lu).", GetLastError());
        return;
    }
    if (WaitForSingleObject(handle, INFINITE) != WAIT_OBJECT_0) {
        bedrock_log_error("WaitForSingleObject failed (error %lu).", GetLastError());
    };
    CloseHandle(handle);
}

u32 bedrock_thread_array_index(
    const bedrock_thread_t *threads, 
    u32                     thread_count)
{
    if (!threads || thread_count <= 0)
        return 0;

    DWORD self = GetCurrentThreadId();
    for (u32 i = 0; i < thread_count; i++) {
        if (self == (DWORD)threads[i])
            return i;
    }
    return 0;
}

bedrock_thread_t bedrock_thread_current(void)
{
    return (bedrock_thread_t)GetCurrentThreadId();
}


void bedrock_thread_affinity(
    u8                 *stack_memory, 
    bedrock_thread_t   *threads, 
    u32                 thread_count, 
    u32                 cpu_count,
    u32                 begin_cpu_index)
{
    bedrock_assert_debug(temp_stack_memory && threads);

    u32 i = 0;
    u32 j = begin_cpu_index;
    while (i < thread_count && j < cpu_count) {
        DWORD thread_id = (DWORD)threads[i];
        HANDLE handle = OpenThread(THREAD_ALL_ACCESS, FALSE, handle);
        if (!handle) {
            bedrock_log_error("Failed to open thread handle for setting CPU affinity, for %u thread, %u CPU: %lu.", i, j, GetLastError());
            return;
        }
        DWORD_PTR mask = (DWORD_PTR)1 << j;
        DWORD_PTR prev_mask = SetThreadAffinityMask(handle, mask);
        if (!prev_mask) {
            bedrock_log_error("SetThreadAffinityMask failed (error %lu) for CPU %u", GetLastError(), j);
            CloseHandle(handle);
            return;
        }
        CloseHandle(handle);
        i++; j++;
    }
}
#endif /* LAKE_PLATFORM_WINDOWS */
