#include <amw/bedrock/process.h>
#include <amw/bedrock/log.h>

#include "win32_hadal.h"

struct hadal *g_windows = NULL;

RIVEN_ENCORE(hadal, win32)
{
    assert_debug(encore->header.interface && *encore->header.interface == NULL);
}
