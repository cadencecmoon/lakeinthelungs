#pragma once

#include <amw/bedrock.h>

#ifdef __cplusplus
extern "C" {
#endif

typedef s16 wchar16;
typedef s32 wchar32;
#ifdef AMW_USE_WCHAR32_UNICODE
typedef wchar32 wchar;
#else
typedef wchar16 wchar;
#endif

#ifdef __cplusplus
}
#endif
