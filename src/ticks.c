#include <lake/hadal.h>
#include <lake/time.h>

static uint64_t tick_start = 0;
static uint32_t tick_numerator_ms;
static uint32_t tick_denominator_ms;
static uint32_t tick_numerator_ns;
static uint32_t tick_denominator_ns;

static uint32_t calculate_gcd(uint32_t a, uint32_t b)
{
    if (b == 0)
        return a;
    return calculate_gcd(b, (a % b));
}

static void ticks_init(void)
{
    uint64_t tick_freq;
    uint32_t gcd;

    tick_freq = hadal_timer_frequency();
    assert_debug(tick_freq > 0 && tick_freq <= (uint64_t)UINT32_MAX);

    gcd = calculate_gcd(AMW_MS_PER_SECOND, (uint32_t)tick_freq);
    tick_numerator_ms = (AMW_MS_PER_SECOND / gcd);
    tick_denominator_ms = (uint32_t)(tick_freq / gcd);

    gcd = calculate_gcd(AMW_NS_PER_SECOND, (uint32_t)tick_freq);
    tick_numerator_ns = (AMW_NS_PER_SECOND / gcd);
    tick_denominator_ns = (uint32_t)(tick_freq / gcd);

    tick_start = hadal_timer_counter();

    if (!tick_start)
        --tick_start;
}

uint64_t ticks_ms(void)
{
    uint64_t starting_value, value;

    if (!tick_start)
        ticks_init();

    starting_value = (hadal_timer_counter() - tick_start);
    value = (starting_value * tick_numerator_ms);
    assert_debug(value >= starting_value);
    value /= tick_denominator_ms;
    return value;
}

uint64_t ticks_ns(void)
{
    uint64_t starting_value, value;

    if (!tick_start)
        ticks_init();

    starting_value = (hadal_timer_counter() - tick_start);
    value = (starting_value * tick_numerator_ns);
    assert_debug(value >= starting_value);
    value /= tick_denominator_ns;
    return value;
}
