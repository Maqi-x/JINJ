#include <jinj/defs.h>
#include <stdlib.h>
#include <string.h>
static inline void* _jinj_memdup(const void* mem, jinj_usize_t size) {
    void* result = malloc(size);
    if (result != NULL) {
        memcpy(result, mem, size);
    }
    return result;
}
