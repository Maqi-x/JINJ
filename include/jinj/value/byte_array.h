#include <jinj/defs.h>

struct JinjByteArray;
typedef struct JinjByteArray JinjByteArrayValue;

typedef struct JinjByteArrayBuilder {
    jinj_byte_t* data;
    jinj_usize_t len;
    jinj_usize_t cap;
} JinjByteArrayBuilder;

typedef struct JinjByteArray {
    jinj_byte_t* data;
    jinj_usize_t len;
} JinjByteArray;
