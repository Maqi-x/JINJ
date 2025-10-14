#include <jinj/defs.h>
#include <stdlib.h>
#include <string.h>
static inline char* _jinj_strdup(const char* str) {
    jinj_usize_t len = strlen(str);
    char* result = malloc(len + 1);
    if (result != NULL) {
        memcpy(result, str, len + 1);
    }
    return result;
}
