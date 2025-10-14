#include "jinj/detail/jml/vars.h"
#include <jinj/detail/jml.h>

#include <jinj/detail/lib/str_hash.h>
#include <jinj/detail/lib/next_power_of_two.h>
#include <jinj/detail/lib/memdup.h>

#include <memory.h>
#include <stdlib.h>

JinjMlVarsEntry _jinj_ml_vars_entry_make(char* name, jinj_usize_t name_len,
                                         char* value, jinj_usize_t value_len) {
    return (JinjMlVarsEntry) { .name = name, .name_len = name_len,
                               .value = value, .value_len = value_len,
                               .state = JinjMlVarsEntryEmpty };
}

JinjMlVarsEntry _jinj_ml_vars_entry_make_strdup(const char* name, jinj_usize_t name_len,
                                                const char* value, jinj_usize_t value_len) {
    char* name_copy = _jinj_memdup(name, name_len);
    char* value_copy = NULL;
    if (name_copy != NULL)
        value_copy = _jinj_memdup(value, value_len);
    return (JinjMlVarsEntry) { .name = name_copy, .name_len = name_len,
                               .value = value_copy, .value_len = value_len,
                               .state = JinjMlVarsEntryEmpty };
}

static inline jinj_bool_t set_str(char** str, jinj_usize_t* str_len,
                           const char* new_str, jinj_usize_t new_str_len) {
    if (*str && *str_len >= new_str_len) {
        memcpy(*str, new_str, new_str_len);
        *str_len = new_str_len;
        return JINJ_TRUE;
    }

    char* new_str_copy = _jinj_memdup(new_str, new_str_len);
    if (new_str_copy == NULL) return JINJ_FALSE;

    free(*str);
    *str = new_str_copy;
    *str_len = new_str_len;
    return JINJ_TRUE;
}

jinj_bool_t _jinj_ml_vars_entry_set_values(JinjMlVarsEntry* entry,
                                           const char* name, jinj_usize_t name_len,
                                           const char* value, jinj_usize_t value_len) {
    return set_str(&entry->name, &entry->name_len, name, name_len)
        && set_str(&entry->value, &entry->value_len, value, value_len);
}

JinjMlVarsEntry* _jinj_alloc_ml_vars_entries_uninit(jinj_usize_t count) {
    return malloc(count * sizeof(JinjMlVarsEntry));
}

JinjMlVarsEntry* _jinj_alloc_ml_vars_entries_zeroed(jinj_usize_t count) {
    return calloc(count, sizeof(JinjMlVarsEntry));
}

void _jinj_free_ml_vars_entries(JinjMlVarsEntry* entries) {
    return free(entries);
}

jinj_usize_t _jinj_ml_vars_compute_index(JinjMlVars* vars, const char* key, jinj_usize_t len) {
    return _jinj_str_hash(key, len) & (vars->cap - 1);
}

JinjMlVarsEntry* _jinj_ml_vars_get_entries(JinjMlVars* vars) {
    if (vars->is_on_heap) return vars->heap_data;
    return vars->sbo_buf;
}

const JinjMlVarsEntry* _jinj_ml_vars_get_entries_const(const JinjMlVars* vars) {
    if (vars->is_on_heap) return vars->heap_data;
    return vars->sbo_buf;
}

JinjMlVarsEntry* _jinj_ml_vars_find_entry(JinjMlVars* vars, const char* key, jinj_usize_t len) {
    JinjMlVarsEntry* entries = _jinj_ml_vars_get_entries(vars);

    jinj_usize_t index = _jinj_ml_vars_compute_index(vars, key, len);
    while (entries[index].state != JinjMlVarsEntryEmpty) {
        if (entries[index].state == JinjMlVarsEntryOccupied) {
            jinj_bool_t is_equal = entries[index].name_len == len
                                   && memcmp(entries[index].name, key, len) == 0;
            if (is_equal) return &entries[index];
        }
        index = (index + 1) & (vars->cap - 1);
    }
    return NULL;
}

const JinjMlVarsEntry* _jinj_ml_vars_find_entry_const(const JinjMlVars* vars, const char* key, jinj_usize_t len) {
    return (const JinjMlVarsEntry*) _jinj_ml_vars_find_entry((JinjMlVars*) vars, key, len);
}

JinjMlVarsEntry* _jinj_ml_vars_get_new_entry(const char* key, jinj_usize_t len);

jinj_bool_t _jinj_ml_vars_rehash(JinjMlVars* vars, jinj_usize_t new_cap) {
    jinj_usize_t old_cap = vars->cap;
    jinj_bool_t old_is_on_heap = vars->is_on_heap;
    JinjMlVarsEntry* old_entries = _jinj_ml_vars_get_entries(vars);

    vars->cap = _jinj_next_power_of_two(new_cap);

    if (vars->cap < JINJ_ML_VARS_TABLE_SBO_SIZE) {
        vars->cap = JINJ_ML_VARS_TABLE_SBO_SIZE;
    }

    vars->is_on_heap = (vars->cap != JINJ_ML_VARS_TABLE_SBO_SIZE);

    if (!vars->is_on_heap && !old_is_on_heap) {
        return JINJ_TRUE;
    }

    JinjMlVarsEntry* new_entries;
    if (vars->is_on_heap) {
        new_entries = _jinj_alloc_ml_vars_entries_zeroed(vars->cap);
        if (new_entries == NULL) return JINJ_FALSE;
    } else {
        new_entries = vars->sbo_buf;
        memset(new_entries, 0, sizeof(vars->sbo_buf));
    }

    for (jinj_usize_t i = 0; i < old_cap; ++i) {
        if (old_entries[i].state != JinjMlVarsEntryOccupied) continue;

        jinj_usize_t index = _jinj_ml_vars_compute_index(vars, old_entries[i].name,
                                                         old_entries[i].name_len);

        while (new_entries[index].state == JinjMlVarsEntryOccupied) {
            index = (index + 1) & (vars->cap - 1); // linear probing
        }
        new_entries[index] = old_entries[i];
    }

    if (old_is_on_heap) _jinj_free_ml_vars_entries(vars->heap_data);
    if (vars->is_on_heap) vars->heap_data = new_entries;

    vars->num_tombstones = 0;
    return JINJ_TRUE;
}

jinj_bool_t _jinj_ml_vars_resize_if_needed(JinjMlVars* vars, jinj_usize_t num_new_elements) {
    jinj_usize_t new_size = vars->size + num_new_elements;
    if ((new_size + vars->num_tombstones) * 4 <= vars->cap * 3) {
        return JINJ_TRUE;
    }

    jinj_usize_t cap_doubled = vars->cap * 2;
    jinj_usize_t resize_to = new_size > cap_doubled ? new_size : cap_doubled;

    return _jinj_ml_vars_rehash(vars, resize_to);
}

void jinj_ml_vars_init(JinjMlVars* vars) {
    vars->cap = JINJ_ML_VARS_TABLE_SBO_SIZE;
    vars->size = 0;
    memset(vars->sbo_buf, 0, sizeof(vars->sbo_buf));
    vars->is_on_heap = JINJ_FALSE;
}

void jinj_ml_vars_deinit(JinjMlVars* vars) {
    JinjMlVarsEntry* entries = _jinj_ml_vars_get_entries(vars);
    for (jinj_usize_t i = 0; i < vars->cap; ++i) {
        if (entries[i].state == JinjMlVarsEntryOccupied) {
            _jinj_ml_vars_entry_free(&entries[i]);
        }
    }

    if (vars->is_on_heap) {
        _jinj_free_ml_vars_entries(vars->heap_data);
    }
}

JinjMlVarsOpResult jinj_ml_vars_contains(JinjMlVars* vars, const char* key, jinj_usize_t len) {
    JinjMlVarsEntry* entries = _jinj_ml_vars_get_entries(vars);

    jinj_usize_t index = _jinj_ml_vars_compute_index(vars, key, len);
    while (entries[index].state != JinjMlVarsEntryEmpty) {
        if (entries[index].state == JinjMlVarsEntryOccupied) {
            jinj_bool_t is_equal = entries[index].name_len == len
                                   && memcmp(entries[index].name, key, len) == 0;
            if (is_equal) return JINJ_TRUE;
        }
        index = (index + 1) & (vars->cap - 1);
    }

    return JINJ_FALSE;
}

JinjMlVarsOpResult jinj_ml_vars_put(JinjMlVars* vars, const char* name, jinj_usize_t name_len,
                                    const char* value, jinj_usize_t value_len) {
    if (!_jinj_ml_vars_resize_if_needed(vars, 1)) {
        return JinjMlVarsOpAllocFailed;
    }

    if (jinj_ml_vars_contains(vars, name, name_len)) {
        return JinjMlVarsOpKeyAlredyExists;
    }

    JinjMlVarsEntry* entries = _jinj_ml_vars_get_entries(vars);
    
    jinj_usize_t index = _jinj_ml_vars_compute_index(vars, name, name_len);
    JinjMlVarsEntry* first_tombstone = NULL;
    
    while (entries[index].state != JinjMlVarsEntryEmpty) {
        if (entries[index].state == JinjMlVarsEntryOccupied &&
            entries[index].name_len == name_len &&
            memcmp(entries[index].name, name, name_len) == 0) {
            return JinjMlVarsOpKeyAlredyExists;
        }
        if (entries[index].state == JinjMlVarsEntryDeleted && first_tombstone == NULL)
            first_tombstone = &entries[index];
        index = (index + 1) & (vars->cap - 1);
    }
    
    if (first_tombstone) index = first_tombstone - entries;

    if (!_jinj_ml_vars_entry_set_values(&entries[index], name, name_len, value, value_len)) {
        return JinjMlVarsOpAllocFailed;
    }

    entries[index].state = JinjMlVarsEntryOccupied;
    vars->size++;

    return JinjMlVarsOpSuccess;
}

JinjMlVarsOpResult jinj_ml_vars_set(JinjMlVars* vars, const char* name, jinj_usize_t name_len,
                                    const char* value, jinj_usize_t value_len) {
    JinjMlVarsEntry* entry = _jinj_ml_vars_find_entry(vars, name, name_len);
    if (entry == NULL) {
        return JinjMlVarsOpKeyNotFound;
    }

    if (!_jinj_ml_vars_entry_set_values(entry, name, name_len, value, value_len)) {
        return JinjMlVarsOpAllocFailed;
    }

    return JinjMlVarsOpSuccess;
}

JinjMlVarsOpResult jinj_ml_vars_put_or_set(JinjMlVars* vars, const char* name, jinj_usize_t name_len,
                                           const char* value, jinj_usize_t value_len);

JinjMlVarsOpResult jinj_ml_vars_get(const JinjMlVars* vars, const char* name, jinj_usize_t name_len,
                                    const char** out_value, jinj_usize_t* out_value_len) {
    const JinjMlVarsEntry* entry = _jinj_ml_vars_find_entry_const(vars, name, name_len);
    if (entry == NULL) {
        return JinjMlVarsOpKeyNotFound;
    }

    *out_value = entry->value;
    *out_value_len = entry->value_len;
    return JinjMlVarsOpSuccess;
}

JinjMlVarsOpResult jinj_ml_vars_del(JinjMlVars* vars, const char* name, jinj_usize_t name_len) {
    JinjMlVarsEntry* entry = _jinj_ml_vars_find_entry(vars, name, name_len);
    if (entry == NULL) {
        return JinjMlVarsOpKeyNotFound;
    }

    _jinj_ml_vars_entry_free(entry);
    *entry = (JinjMlVarsEntry) {0};
    entry->state = JinjMlVarsEntryDeleted;

    vars->size--;
    vars->num_tombstones++;
    return JinjMlVarsOpSuccess;
}

JinjMlVarsOpResult jinj_ml_vars_take_overship_and_del(JinjMlVars* vars, const char* name, jinj_usize_t name_len,
                                                      char** out_name, jinj_usize_t* out_name_len,
                                                      char** out_value, jinj_usize_t* out_value_len)
{
    JinjMlVarsEntry* entry = _jinj_ml_vars_find_entry(vars, name, name_len);
    if (entry == NULL) {
        return JinjMlVarsOpKeyNotFound;
    }

    *out_name = entry->name;
    *out_name_len = entry->name_len;

    *out_value = entry->value;
    *out_value_len = entry->value_len;

    *entry = (JinjMlVarsEntry) {0};
    entry->state = JinjMlVarsEntryDeleted;


    vars->size--;
    vars->num_tombstones++;
    return JinjMlVarsOpSuccess;
}

JinjMlVarsOpResult jinj_ml_vars_reserve(JinjMlVars* vars, jinj_usize_t min_cap) {
    jinj_usize_t cap_doubled = vars->cap * 2;
    jinj_usize_t resize_to = min_cap > cap_doubled ? min_cap : cap_doubled;

    return _jinj_ml_vars_rehash(vars, resize_to);
}

JinjMlVarsOpResult jinj_ml_vars_reserve_exact(JinjMlVars* vars, jinj_usize_t min_cap) {
    if (min_cap < vars->cap) {
        return _jinj_ml_vars_rehash(vars, min_cap);
    }
    return JinjMlVarsOpSuccess;
}
