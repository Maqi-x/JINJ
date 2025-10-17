#include <jinj/defs.h>

struct JinjTable;
typedef struct JinjTable JinjTableValue;

struct JinjValue;
typedef struct JinjValue JinjValue;

typedef struct _JinjTableEntry {
    enum _JinjTableEntryState {
        _JinjTableEntryEmpty,
        _JinjTableEntryOccupied,
        _JinjTableEntryDeleted,
    } state;
} _JinjTableEntry;

typedef struct JinjTable {
    _JinjTableEntry* entries;
} JinjTable;

typedef struct JinjTableIter {
} JinjTableIter;
