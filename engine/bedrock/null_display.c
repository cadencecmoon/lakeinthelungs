#include <amwe/hadal.h>

#if defined(LAKE_PLATFORM_WINDOWS)
FN_RIVEN_ENCORE_STUB(hadal, win32)
#elif defined(LAKE_PLATFORM_APPLE_MACOS)
FN_RIVEN_ENCORE_STUB(hadal, cocoa)
#elif defined(LAKE_PLATFORM_APPLE_IOS)
FN_RIVEN_ENCORE_STUB(hadal, uikit)
#elif defined(LAKE_PLATFORM_ANDROID)
FN_RIVEN_ENCORE_STUB(hadal, android)
#elif defined(LAKE_PLATFORM_EMSCRIPTEN)
FN_RIVEN_ENCORE_STUB(hadal, html5)
#endif /* LAKE_PLATFORM_WINDOWS */
#ifdef HADAL_XCB
FN_RIVEN_ENCORE_STUB(hadal, xcb)
#endif /* HADAL_XCB */
#ifdef HADAL_DRMKMS
FN_RIVEN_ENCORE_STUB(hadal, drmkms)
#endif /* HADAL_DRMKMS */

/* TODO */
FN_RIVEN_ENCORE_STUB(hadal, null)
