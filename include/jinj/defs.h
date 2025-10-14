#include <stddef.h>
#include <stdint.h>

#if defined(bool)
    typedef bool jinj_bool_t;
#else
    typedef _Bool jinj_bool_t;
#endif
#define JINJ_TRUE ((jinj_bool_t)1)
#define JINJ_FALSE ((jinj_bool_t)0)

typedef size_t jinj_usize_t;
typedef uint32_t jinj_rune_t;
typedef uint64_t jinj_hash_t;

#if defined(__GNUC__) || defined(__clang__)
#   define _JINJ_ATTR_MALLOC __attribute__((malloc))
#else
#   define _JINJ_ATTR_MALLOC
#endif

#define JINJ_GET_OPTIONAL_TYPE(T) \
     struct { T value; jinj_bool_t has_value; }

#define JINJ_DEFINE_OPTIONAL(T, Name) \
    typedef JINJ_GET_OPTIONAL_TYPE(T) Name;

#define JINJ_OPTIONAL_NONE(Name) ((Name) { .has_value = JINJ_FALSE })
#define JINJ_OPTIONAL_SOME(Name, val) ((Name) { .value = val, .has_value = JINJ_TRUE })

