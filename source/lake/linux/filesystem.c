#include "../filesystem.h"
#include "../utility.h"
#include "../sys.h"

#ifdef LAKE_PLATFORM_LINUX
#include <sys/types.h>
#include <unistd.h>

char const *lake_fs_executable_path(void)
{
    static char exepath[256] = {'\0'};

    if (exepath[0] == '\0') {
        ssize bufsize = readlink("/proc/self/exe", exepath, 256);
        if (bufsize == -1)
            return nullptr;
        lake_dbg_assert(bufsize < 256, LAKE_ERROR_OUT_OF_RANGE, nullptr);
        exepath[bufsize] = '\0';
    }
    return exepath;
}
#endif /* LAKE_PLATFORM_LINUX */
