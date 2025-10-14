#ifndef JINJ_DETAIL_JML_VARS_H
#define JINJ_DETAIL_JML_VARS_H

#include <jinj/defs.h>

#include "var.h"

typedef enum JinjMlVarsOpResult {
    JinjMlVarsOpSuccess,
    JinjMlVarsOpAllocFailed,
    JinjMlVarsOpKeyNotFound,
    JinjMlVarsOpKeyAlredyExists,
} JinjMlVarsOpResult;

typedef struct JinjMlVarsEntry {
    char* name;
    jinj_usize_t name_len;

    char* value;
    jinj_usize_t value_len;

    enum JinjMlVarsEntryState {
        JinjMlVarsEntryEmpty,
        JinjMlVarsEntryOccupied,
        JinjMlVarsEntryDeleted,
    } state;
} JinjMlVarsEntry;

JinjMlVarsEntry _jinj_ml_vars_entry_make(char* name, jinj_usize_t name_len, char* value, jinj_usize_t value_len);
JinjMlVarsEntry _jinj_ml_vars_entry_make_strdup(const char* name, jinj_usize_t name_len,
                                               const char* value, jinj_usize_t value_len);

void _jinj_ml_vars_entry_free(JinjMlVarsEntry* entry);

JinjMlVarsEntry* _jinj_alloc_ml_vars_entries_uninit(jinj_usize_t count);
JinjMlVarsEntry* _jinj_alloc_ml_vars_entries_zeroed(jinj_usize_t count);
void _jinj_free_ml_vars_entries(JinjMlVarsEntry* entries);

#ifndef JINJ_ML_VARS_TABLE_SBO_SIZE
#   define JINJ_ML_VARS_TABLE_SBO_SIZE 128
#endif

typedef struct JinjMlVars {
    union {
        JinjMlVarsEntry* heap_data;
        JinjMlVarsEntry sbo_buf[JINJ_ML_VARS_TABLE_SBO_SIZE];
    };
    jinj_bool_t is_on_heap;
    jinj_usize_t size;
    jinj_usize_t cap;
    jinj_usize_t num_tombstones;
} JinjMlVars;

jinj_usize_t _jinj_ml_vars_compute_index(JinjMlVars* vars, const char* key, jinj_usize_t len);
JinjMlVarsEntry* _jinj_ml_vars_get_entries(JinjMlVars* vars);
const JinjMlVarsEntry* _jinj_ml_vars_get_entries_const(const JinjMlVars* vars);

JinjMlVarsEntry* _jinj_ml_vars_find_entry(JinjMlVars* vars, const char* key, jinj_usize_t len);
const JinjMlVarsEntry* _jinj_ml_vars_find_entry_const(const JinjMlVars* vars, const char* key, jinj_usize_t len);

jinj_bool_t _jinj_ml_vars_rehash(JinjMlVars* vars, jinj_usize_t new_cap);
jinj_bool_t _jinj_ml_vars_resize_if_needed(JinjMlVars* vars, jinj_usize_t num_new_elements);

void jinj_ml_vars_init(JinjMlVars* vars);
void jinj_ml_vars_deinit(JinjMlVars* vars);

JinjMlVarsOpResult jinj_ml_vars_contains(JinjMlVars* vars, const char* key, jinj_usize_t len);

JinjMlVarsOpResult jinj_ml_vars_put(JinjMlVars* vars, const char* name, jinj_usize_t name_len,
                                    const char* value, jinj_usize_t value_len);

JinjMlVarsOpResult jinj_ml_vars_set(JinjMlVars* vars, const char* name, jinj_usize_t name_len,
                                    const char* value, jinj_usize_t value_len);

JinjMlVarsOpResult jinj_ml_vars_put_or_set(JinjMlVars* vars, const char* name, jinj_usize_t name_len,
                                           const char* value, jinj_usize_t value_len);

JinjMlVarsOpResult jinj_ml_vars_get(const JinjMlVars* vars, const char* name, jinj_usize_t name_len,
                                    const char** out_value, jinj_usize_t* out_value_len);

JinjMlVarsOpResult jinj_ml_vars_del(JinjMlVars* vars, const char* name, jinj_usize_t name_len);


JinjMlVarsOpResult jinj_ml_vars_take_overship_and_del(JinjMlVars* vars, const char* name, jinj_usize_t name_len,
                                                      char** out_name, jinj_usize_t* out_name_len,
                                                      char** out_value, jinj_usize_t* out_value_len);

JinjMlVarsOpResult jinj_ml_vars_reserve(JinjMlVars* vars, jinj_usize_t min_cap);
JinjMlVarsOpResult jinj_ml_vars_reserve_exact(JinjMlVars* vars, jinj_usize_t min_cap);

#endif // JINJ_DETAIL_JML_VARS_H
