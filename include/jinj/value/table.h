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

#if !defined(JINJ_TABLE_SBO_SIZE)
#   define JINJ_TABLE_SBO_SIZE 16 // most tables have no more than 16 keys;
                                  // setting a large value here may cause high stack memory usage since this type is used in the JinjValueData union
#endif

typedef struct JinjTable {
    _JinjTableEntry* entries;
} JinjTable;

typedef struct JinjTableIter {
} JinjTableIter;
