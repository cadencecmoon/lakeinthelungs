#include <amw/aria/core.h>

f32 aria_floorf(f32 x)
{
    union { f32 f; u32 i; } u = {x};

    s32 exponent = (s32)(u.i >> 23 & 0xff) - 0x7f;
    u32 mantissa;

    if (exponent >= 23)
        return x;
    if (exponent >= 0) {
        mantissa = 0x007fffffu >> exponent;
        if ((u.i & mantissa) == 0)
            return x;
        ARIA_FORCE_EVAL(x + 0x1p120f);
        if (u.i >> 31)
            u.i += mantissa;
        u.i &= ~mantissa;
    } else {
        ARIA_FORCE_EVAL(x + 0x1p120f);
        if (u.i >> 31 == 0)
            u.i = 0;
        else if (u.i << 1)
            u.f = -1.0;
    }
    return u.f;
}
