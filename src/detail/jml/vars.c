#include <jinj/detail/jml.h>

jinj_bool_t _jinj_ml_vars_need_resize(JinjMlVars* vars, jinj_usize_t num_new_elements);

JinjMlVarsOpResult jinj_ml_vars_init(JinjMlVars* vars, jinj_usize_t initial_capacity);
void jinj_ml_vars_deinit(JinjMlVars* vars);

JinjMlVarsOpResult jinj_ml_vars_put(JinjMlVars* vars, const char* key, const char* value);

JinjMlVarsOpResult jinj_ml_vars_set(JinjMlVars* vars, const char* key, const char* value);
JinjMlVarsOpResult jinj_ml_vars_get(JinjMlVars* vars, const char* key, const char** out);

JinjMlVarsOpResult jinj_ml_vars_reserve(JinjMlVars* vars, jinj_usize_t min_cap);
JinjMlVarsOpResult jinj_ml_vars_reserve_exact(JinjMlVars* vars, jinj_usize_t min_cap);
