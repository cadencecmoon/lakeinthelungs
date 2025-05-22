#include "../utility.h"
#include "../sys.h"

#if defined(LAKE_PLATFORM_LINUX) || defined(LAKE_PLATFORM_ANDROID)
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>

void lake_sys_cpuinfo(s32 *out_threads, s32 *out_cores, s32 *out_packages)
{
    static s32 threads, cores, packages = 0;

    if (packages != 0)
        goto output;
    threads = 1; cores = 1; packages = 1;

    s32 fd, len, pos, end;
    char buf[4096];
    char num[100];
    s32 numsize = sizeof(num);
    char const *err = "Failed parsing /proc/cpuinfo.";

    fd = open("/proc/cpuinfo", O_RDONLY);
    if (fd != -1) {
        len = read(fd, buf, 4096);
        close(fd);
        pos = 0;
        while (pos < len) {
            if (!lake_strncmp(buf + pos, "cpu cores", 9)) {
                pos = lake_strchr(buf + pos, ':') - buf + 2;
                end = lake_strchr(buf + pos, '\n') - buf;

                if (pos < len && end < len) {
                    lake_strncpy(num, buf + pos, numsize);

                    num[numsize - 1] = 0; /* [-Wstringop-truncation] */
                    lake_assert((end - pos) > 0 && (end - pos) < numsize, LAKE_ERROR_OUT_OF_RANGE, "CPU num buffer too small.");
                    num[end - pos] = '\0'; /* because the compiler keeps fucking screaming */

                    s32 processor = atoi(num);
                    if ((processor) > cores)
                        cores = processor;
                } else {
                    lake_error("%s", err);
                    break;
                }
            } else if (!lake_strncmp(buf + pos, "siblings", 8)) {
                pos = lake_strchr(buf + pos, ':') - buf + 2;
                end = lake_strchr(buf + pos, '\n') - buf;

                if (pos < len && end < len) {
                    lake_strncpy(num, buf + pos, numsize);

                    num[numsize - 1] = 0; /* [-Wstringop-truncation] */
                    lake_assert((end - pos) > 0 && (end - pos) < numsize, LAKE_ERROR_OUT_OF_RANGE, "CPU num buffer too small.");
                    num[end - pos] = '\0'; /* ... */

                    s32 core_id = atoi(num);
                    if ((core_id) > threads)
                        threads = core_id;
                } else {
                    lake_error("%s", err);
                    break;
                }
            }
            pos = lake_strchr(buf + pos, '\n') - buf + 1;
        }
    } else {
        lake_error("%s", err);
        cores = sysconf(_SC_NPROCESSORS_CONF);
        threads = 2 * cores;
    }
output:
    if (out_threads)  *out_threads = threads;
    if (out_cores)    *out_cores = cores;
    if (out_packages) *out_packages = packages;
}

void lake_sys_meminfo(usize *out_total_ram, usize *out_pagesize)
{
    ssize page, bytes;
    ssize count = sysconf(_SC_PHYS_PAGES);
    if (count == -1)
        lake_error("sysconf _SC_PHYS_PAGES failed.");
    page = sysconf(_SC_PAGE_SIZE);
    if (page == -1)
        lake_error("sysconf _SC_PAGE_SIZE failed.");
    bytes = page * count;
    if (out_total_ram) *out_total_ram = bytes;
    if (out_pagesize)  *out_pagesize = page;
}

void lake_sys_hugetlbinfo(usize *out_huge_pagesize, usize max_accept_size)
{
    DIR *dir;
    struct dirent *entry;
    usize count, bytes = -1;

    if (out_huge_pagesize == nullptr)
        return;

    dir = opendir("/sys/kernel/mm/hugepages/");
    if (dir) {
        while ((entry = readdir(dir))) {
            char *name = entry->d_name;
            char *end;

            if (lake_strncmp(name, "hugepages-", 10) != 0)
                continue;
            name += 10;

            count = strtol(name, &end, 10);
            if (*end == 'k' && *(end + 1) == 'B') {
                /* We will accept huge pages of up to max_target_hugepagesize. */
                usize mb = count / 1024;
                if (mb <= max_accept_size) {
                    bytes = count * 1024;
                }
            }
        }
        closedir(dir);
    }
    *out_huge_pagesize = bytes;
}
#endif /* LAKE_PLATFORM_LINUX || LAKE_PLATFORM_ANDROID */
