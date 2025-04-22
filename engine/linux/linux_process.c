#include <amwe/bedrock.h>

#if defined(LAKE_PLATFORM_LINUX)
#include <fcntl.h>
#include <unistd.h>
#include <dirent.h>
#include <sys/stat.h>
#include <sys/types.h>
#include <string.h> /* strncmp */

void bedrock_cpuinfo(s32 *out_threads, s32 *out_cores, s32 *out_packages)
{
    static s32 threads, cores, packages = 0;

    s32 fd, len, pos, end;
    char buf[4096];
    char num[100];

    if (packages != 0) goto result_cpuinfo;

    threads = 1;
    cores = 1;
    packages = 1;

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
                    strncpy(num, buf + pos, lake_sizeof(num));
                    num[100-1] = 0; /* because the compiler screams at me [-Wstringop-truncation] */
                    bedrock_assert_debug((end - pos) > 0 && (end - pos) < (s32)lake_sizeof(num));
                    num[end - pos] = '\0';

                    s32 processor = atoi(num);
                    if ((processor) > cores) {
                        cores = processor;
                    }
                } else {
                    bedrock_log_error("failed parsing /proc/cpuinfo");
                    break;
                }
            } else if (!strncmp(buf + pos, "siblings", 8)) {
                pos = strchr(buf + pos, ':') - buf + 2;
                end = strchr(buf + pos, '\n') - buf;

                if (pos < len && end < len) {
                    strncpy(num, buf + pos, sizeof(num));
                    num[100-1] = 0; /* because the compiler screams at me [-Wstringop-truncation] */
                    bedrock_assert_debug((end - pos) > 0 && (end - pos) < (s32)lake_sizeof(num));
                    num[end - pos] = '\0'; /* because it keeps fucking screaming */

                    s32 core_id = atoi(num);
                    if ((core_id) > threads) {
                        threads = core_id;
                    }
                } else {
                    bedrock_log_error("failed parsing /proc/cpuinfo");
                    break;
                }
            }
            pos = strchr(buf + pos, '\n') - buf + 1;
        }
    } else {
        bedrock_log_error("failed parsing /proc/cpuinfo");
        cores = sysconf(_SC_NPROCESSORS_CONF);
        threads = 2 * cores;
    }

result_cpuinfo:
    if (out_threads)  *out_threads = threads;
    if (out_cores)    *out_cores = cores;
    if (out_packages) *out_packages = packages;
}

void bedrock_meminfo(usize *out_total_ram, usize *out_pagesize)
{
    ssize page, bytes;

    ssize count = sysconf(_SC_PHYS_PAGES);
    if (count == -1)
        bedrock_log_error("lake_sys_meminfo: sysconf _SC_PHYS_PAGES failed");

    page = sysconf(_SC_PAGE_SIZE);
    if (page == -1)
        bedrock_log_error("lake_sys_meminfo: sysconf _SC_PAGE_SIZE failed");

    bytes = page * count;

    if (out_total_ram) *out_total_ram = bytes;
    if (out_pagesize)  *out_pagesize = page;
}

void bedrock_hugetlbinfo(usize *out_huge_pagesize, usize max_accept_size)
{
    if (!out_huge_pagesize) return;

    DIR *dir;
    struct dirent *entry;
    usize count, bytes;

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
#endif /* LAKE_PLATFORM_LINUX */
