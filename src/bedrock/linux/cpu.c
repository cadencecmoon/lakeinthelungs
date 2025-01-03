#include <lake/bedrock/parser.h>
#include <lake/bedrock/log.h>
#include <lake/bedrock/os.h>

#include <string.h>
#include <fcntl.h>
#include <unistd.h>
#include <sys/stat.h>
#include <sys/types.h>

static s32 cpu_threads, cpu_cores, cpu_packages = 0;

AMWAPI void bedrock_cpu_count(u32 *threads, u32 *cores, u32 *packages)
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
