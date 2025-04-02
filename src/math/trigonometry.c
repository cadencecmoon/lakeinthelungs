#include <amw/aria/core.h>

static const f32 g_B = 4.f / ARIA_PI;
static const f32 g_C = -4.f / (ARIA_PI * ARIA_PI);
static const f32 g_P = 0.225f;

f32 aria_sinf(f32 x)
{
    f32 y = g_B * x + g_C * x * (x < 0 ? -x : x);
    return g_P * (y * (y < 0 ? -y : y) - y) + y;
}

f32 aria_cosf(f32 x)
{
    x = (x > 0) ? -x : x;
    x += ARIA_PI_2;
    return aria_sinf(x);
}
