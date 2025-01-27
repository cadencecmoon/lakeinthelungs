#include "../../bedrock.h"

#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>

#include <string.h>

void bedrock_cpuinfo(ssize *threads, ssize *cores, ssize *packages)
{
    static ssize cpu_threads, cpu_cores, cpu_packages = 0;
    s32 fd, len, pos, end;
    char buf[4096];
    char num[100];

    if (cpu_packages != 0) goto result_cpuinfo;

    cpu_threads = 1;
    cpu_cores = 1;
    cpu_packages = 1;

    fd = open("/proc/cpuinfo", O_RDONLY);
    if (fd != -1) {
        len = read(fd, buf, 4096);
        close(fd);
        pos = 0;
        while (pos < len) {
            if (!strncmp(buf + pos, "cpu cores", 9)) {
                pos = strchr(buf + pos, ':') - buf + 2;
                end = strchr(buf + pos, '\n') - buf;

                if (pos < len && end < len) {
                    strncpy(num, buf + pos, sizeof(num));
                    num[100-1] = 0; /* because the compiler screams at me [-Wstringop-truncation] */
                    assert_debug((end - pos) > 0 && (end - pos) < (s32)sizeof(num));
                    num[end - pos] = '\0';

                    s32 processor = atoi(num);
                    if ((processor) > cpu_cores) {
                        cpu_cores = processor;
                    }
                } else {
                    log_error("failed parsing /proc/cpuinfo");
                    break;
                }
            } else if (!strncmp(buf + pos, "siblings", 8)) {
                pos = strchr(buf + pos, ':') - buf + 2;
                end = strchr(buf + pos, '\n') - buf;

                if (pos < len && end < len) {
                    strncpy(num, buf + pos, sizeof(num));
                    num[100-1] = 0; /* because the compiler screams at me [-Wstringop-truncation] */
                    assert_debug((end - pos) > 0 && (end - pos) < (s32)sizeof(num));
                    num[end - pos] = '\0'; /* because it keeps fucking screaming */

                    s32 core_id = atoi(num);
                    if ((core_id) > cpu_threads) {
                        cpu_threads = core_id;
                    }
                } else {
                    log_error("failed parsing /proc/cpuinfo");
                    break;
                }
            }
            pos = strchr(buf + pos, '\n') - buf + 1;
        }
    } else {
        log_error("failed parsing /proc/cpuinfo");
        cpu_cores = sysconf(_SC_NPROCESSORS_CONF);
        cpu_threads = 2 * cpu_cores;
    }

result_cpuinfo:
    if (threads)  *threads = cpu_threads;
    if (cores)    *cores = cpu_cores;
    if (packages) *packages = cpu_packages;
}

void bedrock_meminfo(ssize *systemram, ssize *pagesize)
{
    ssize count, page, bytes;

    if (systemram) *systemram = 0;
    if (pagesize)  *pagesize = 0;

    count = sysconf(_SC_PHYS_PAGES);
    if (count == -1) {
        log_error("bedrock_meminfo: sysconf _SC_PHYS_PAGES failed");
        return;
    }

    page = sysconf(_SC_PAGE_SIZE);
    if (page == -1) {
        log_error("bedrock_meminfo: sysconf _SC_PAGE_SIZE failed");
    }

    bytes = page * count;
    if (systemram) *systemram = bytes;
    if (pagesize)  *pagesize = page;
}

void bedrock_hugetlbinfo(ssize *hugepagesize, ssize max_target_hugepagesize)
{
    if (!hugepagesize) return;

    DIR *dir;
    struct dirent *entry;
    ssize count, bytes;

    bytes = -1;
    dir = opendir("/sys/kernel/mm/hugepages/");
    if (dir) {
        while ((entry = readdir(dir))) {
            char *name = entry->d_name;
            char *end;

            if (strncmp(name, "hugepages-", 10) != 0)
                continue;
            name += 10;

            count = strtol(name, &end, 10);
            if (*end == 'k' && *(end + 1) == 'B') {
                /* We will accept huge pages of up to max_target_hugepagesize. */
                ssize mb = count / 1024;
                if (mb <= max_target_hugepagesize) {
                    bytes = count * 1024;
                }
            }
        }
        closedir(dir);
    }
    *hugepagesize = bytes > 0 ? bytes : -1;
}
