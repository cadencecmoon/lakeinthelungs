/*  Lake in the Lungs
 *  Copyright (c) 2025 Cadence C. Moon
 *  The source code is licensed under a standard MIT license. */

#include <lake/bedrock/assert.h>
#include <lake/bedrock/log.h>
#include <lake/bedrock/os.h>
#include <lake/bedrock/parser.h>

#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <dirent.h>

#include <stdlib.h>
#include <string.h>

static s32 cpu_threads, cpu_cores, cpu_packages = 0;

AMWAPI void bedrock_cpuinfo(u32 *threads, u32 *cores, u32 *packages)
{
    if (cpu_packages != 0) goto get_count;

    s32 fd, len, pos, end;
    char buf[4096];
    char num[100];

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

                    s32 processor = parser_atoi(num);
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

                    s32 core_id = parser_atoi(num);
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

get_count:
    if (threads)  *threads = (u32)cpu_threads;
    if (cores)    *cores = (u32)cpu_cores;
    if (packages) *packages = (u32)cpu_packages;
}

AMWAPI void bedrock_meminfo(usize *out_systemram, usize *out_pagesize, usize *out_hugepagesize)
{
    ssize count, page_size, bytes;

    if (out_systemram)    *out_systemram = 0;
    if (out_pagesize)     *out_pagesize = 0;
    if (out_hugepagesize) *out_hugepagesize = 0;

    count = sysconf(_SC_PHYS_PAGES);
    if (count == -1) {
        log_error("bedrock_meminfo: sysconf _SC_PHYS_PAGES failed");
        return;
    }

    page_size = sysconf(_SC_PAGE_SIZE);
    if (page_size == -1) {
        log_error("bedrock_meminfo: sysconf _SC_PAGE_SIZE failed");
    }

    bytes = count * page_size;
    bytes = (bytes + 8) & ~15;

    if (out_systemram)    *out_systemram = bytes;
    if (out_pagesize)     *out_pagesize = page_size;
    if (out_hugepagesize) {
        DIR *dir;
        struct dirent *entry;

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
                    /* We will accept huge pages of sizes 1MiB, 2MiB or 4MiB. */
                    ssize mb = count / 1024;
                    if (mb >= 1 && mb <= 4) {
                        bytes = count * 1024;
                    }
                }
            }
            closedir(dir);
        }
        *out_hugepagesize = bytes > 0 ? bytes : 0;
    }
}
