#include <jinj/detail/lib/str_hash.h>

jinj_hash_t _jinj_str_hash(const char* str, jinj_usize_t len) {
    uint64_t hash = 14695981039346656037ULL;
    for (jinj_usize_t i = 0; i < len; ++i)
        hash = (hash ^ (unsigned char) str[i]) * 1099511628211ULL;
    return hash;
}
