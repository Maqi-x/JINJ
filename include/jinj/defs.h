#include <stddef.h>
#include <stdint.h>

#if defined(bool)
    typedef bool jinj_bool_t;
#else
    typedef _Bool jinj_bool_t;
#endif

typedef size_t jinj_usize_t;
typedef uint32_t jinj_rune_t;
typedef uint64_t jinj_hash_t;

#if defined(__GNUC__) || defined(__clang__)
#   define _JINJ_ATTR_MALLOC __attribute__((malloc))
#else
#   define _JINJ_ATTR_MALLOC
#endif
