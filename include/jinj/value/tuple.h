
#include <jinj/defs.h>

struct JinjTuple;
typedef struct JinjTuple JinjTupleValue;

struct JinjValue;
typedef struct JinjValue JinjValue;

typedef struct JinjTupleBuilder {
    JinjValue* data;
    jinj_usize_t len;
    jinj_usize_t cap;
} JinjTupleBuilder;

typedef struct JinjTuple {
    JinjValue* data;
    jinj_usize_t num_elements;
} JinjTuple;
