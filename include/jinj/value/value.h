#include "byte.h"
#include "rune.h"
#include "int.h"
#include "float.h"
#include "string.h"
#include "byte_array.h"
#include "list.h"
#include "table.h"

typedef struct JinjValue {
    enum JinjValueType {
        JinjValueTypeByte,
        JinjValueTypeRune,
        JinjValueTypeInt,
        JinjValueTypeFloat,

        JinjValueTypeString,
        JinjValueTypeByteArray,
        JinjValueTypeList,
        JinjValueTypeTable,
    } type;
    union JinjValueData {
        JinjByteValue byte;
        JinjRuneValue rune;
        JinjIntValue i;
        JinjFloatValue f;
        JinjStringValue string;
        JinjByteArrayValue byte_array;
        JinjListValue list;
        JinjTableValue table;
    } data;
} JinjValue;

typedef enum JinjValueType JinjValueType;
typedef union JinjValueData JinjValueData;
