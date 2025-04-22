#if defined(LAKE_CC_CLANG_VERSION)
    #pragma clang diagnostic push
    #pragma clang diagnostic ignored "-Wtautological-compare"
    #pragma clang diagnostic ignored "-Wunused-private-field"
    #pragma clang diagnostic ignored "-Wunused-parameter"
    #pragma clang diagnostic ignored "-Wunused-variable"
    #pragma clang diagnostic ignored "-Wmissing-field-initializers"
    #pragma clang diagnostic ignored "-Wnullability-completeness"
#elif defined(LAKE_CC_GNUC_VERSION)
    #pragma GCC diagnostic push
    #pragma GCC diagnostic ignored "-Wtautological-compare"
    #pragma GCC diagnostic ignored "-Wunused-parameter"
    #pragma GCC diagnostic ignored "-Wunused-variable"
    #pragma GCC diagnostic ignored "-Wmissing-field-initializers"
#elif defined(LAKE_CC_MSVC_VERSION)
    #define VMA_VULKAN_VERSION 1004000 // Vulkan 1.4
    #pragma warning(push, 4)
    #pragma warning(disable: 4127) // conditional expression is constant
    #pragma warning(disable: 4100) // unreferenced formal parameter
    #pragma warning(disable: 4189) // local variable is initialized but not referenced
    #pragma warning(disable: 4324) // structure was padded due to alignment specifier
    #pragma warning(disable: 4820) // 'X': 'N' bytes padding added after data member 'X'
#endif

#define VMA_IMPLEMENTATION
#include "vk_xaku.h"

#if defined(__clang__)
    #pragma clang diagnostic pop
#elif defined(__GNUC__)
    #pragma GCC diagnostic pop
#elif defined(_MSVC_LANG)
    #pragma warning(pop)
#endif
