#include <jinj/defs.h>
static inline jinj_usize_t _jinj_next_power_of_two(jinj_usize_t n) {
    if (n == 0) return 1;
    n--;

    jinj_usize_t shiftCount = sizeof(jinj_usize_t) * 8;
    for (jinj_usize_t shift = 1; shift < shiftCount; shift <<= 1) {
        n |= n >> shift;
    }

    return n + 1;
}
