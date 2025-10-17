#include <jinj/defs.h>

struct JinjString;
typedef struct JinjString JinjStringValue;

typedef struct JinjStringBuilder {
    char* data;
    jinj_usize_t len;
    jinj_usize_t cap;
} JinjStringBuilder;

typedef struct JinjString {
    char* data;
    jinj_usize_t len;
} JinjString;
