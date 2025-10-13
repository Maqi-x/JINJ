#ifndef JINJ_DETAIL_JML_VARS_H
#define JINJ_DETAIL_JML_VARS_H

#include <jinj/defs.h>

#include "var.h"

typedef enum JinjMlVarsOpResult {
    JinjMlVarsOpSuccess,
    JinjMlVarsOpAllocFailed,
    JinjMlVarsOpKeyNotFound,
} JinjMlVarsOpResult;

typedef struct JinjMlVarsEntry {
    char* name;
    jinj_usize_t name_len;

    char* value;
    jinj_usize_t value_len;

    struct JinjMlVarsEntry* next;
} JinjMlVarsEntry;

JinjMlVarsEntry jinj_make_ml_vars_entry(char* name, jinj_usize_t name_len, char* value, jinj_usize_t value_len);
JinjMlVarsEntry jinj_make_ml_vars_entry_strdup(const char* name, jinj_usize_t name_len,
                                               const char* value, jinj_usize_t value_len);
#ifndef JINJ_PP_VARS_TABLE_SBO_SIZE
#   define JINJ_PP_VARS_TABLE_SBO_SIZE 128
#endif

typedef union JinjMlVarsTableData {
    JinjMlVarsEntry** on_heap;
    JinjMlVarsEntry* sbo_buf[JINJ_PP_VARS_TABLE_SBO_SIZE];
} JinjMlVarsTableData;

typedef struct JinjMlVars {
    JinjMlVarsTableData table;
    jinj_usize_t size;
    jinj_usize_t cap;
} JinjMlVars;

jinj_bool_t _jinj_ml_vars_need_resize(JinjMlVars* vars, jinj_usize_t num_new_elements);

JinjMlVarsOpResult jinj_ml_vars_init(JinjMlVars* vars, jinj_usize_t initial_capacity);
void jinj_ml_vars_deinit(JinjMlVars* vars);

JinjMlVarsOpResult jinj_ml_vars_put(JinjMlVars* vars, const char* key, const char* value);

JinjMlVarsOpResult jinj_ml_vars_set(JinjMlVars* vars, const char* key, const char* value);
JinjMlVarsOpResult jinj_ml_vars_get(JinjMlVars* vars, const char* key, const char** out);

JinjMlVarsOpResult jinj_ml_vars_reserve(JinjMlVars* vars, jinj_usize_t min_cap);
JinjMlVarsOpResult jinj_ml_vars_reserve_exact(JinjMlVars* vars, jinj_usize_t min_cap);

#endif // JINJ_DETAIL_JML_VARS_H
