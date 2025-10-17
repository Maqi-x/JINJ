#include <jinj/defs.h>

struct JinjList;
typedef struct JinjList JinjListValue;

struct JinjValue;
typedef struct JinjValue JinjValue;

typedef struct JinjList {
    JinjValue* data;
    jinj_usize_t len;
    jinj_usize_t cap;
} JinjList;
