#include <jinj/detail/lexer.h>

#include <memory.h>
#include <stdlib.h>

#define JINJ_TOKEN_LIST_INITIAL_CAP 64
#define _JINJ_TOKEN_LIST_GROW(cap) ((cap) == 0 ? JINJ_TOKEN_LIST_INITIAL_CAP : (cap) * 4)

#define ALLOC_FAILED_RESULT \
    ((JinjLexerResult) { .code = JinjLexerErrorAllocFailed, .error_details.other = NULL })

JinjToken* _jinj_token_list_resize(JinjTokenList* tl, jinj_usize_t new_cap) {
    JinjToken* new_tokens = malloc(new_cap * sizeof(JinjToken));
    if (new_tokens != NULL) {
        if (tl->tokens != NULL) {
            memcpy(new_tokens, tl->tokens, tl->len * sizeof(JinjToken));
            free(tl->tokens);
        }

        tl->tokens = new_tokens;
        tl->cap = new_cap;
    }
    return new_tokens;
}

void jinj_token_list_init(JinjTokenList* tl) {
    tl->tokens = NULL;
    tl->len = 0;
    tl->cap = 0;
}

void jinj_token_list_deinit(JinjTokenList* tl) {
    free(tl->tokens);
    tl->len = 0;
    tl->cap = 0;
}

JinjLexerResult jinj_token_list_append(JinjTokenList* tl, JinjToken tok) {
    if (tl->len == tl->cap) {
        if (_jinj_token_list_resize(tl, _JINJ_TOKEN_LIST_GROW(tl->cap)) == NULL) {
            return ALLOC_FAILED_RESULT;
        }
    }

    tl->tokens[tl->len++] = tok;
    return JINJ_LEXER_RESULT_SUCCESS;
}

JinjLexerResult jinj_token_list_append_new(JinjTokenList* tl, JinjTokenType type, int line, int column) {
    return jinj_token_list_append(tl, jinj_make_token(type, line, column));
}

JinjLexerResult jinj_token_list_append_new_with_value(JinjTokenList* tl, JinjTokenType type, int line, int column,
                                                      const char* value, jinj_usize_t value_len) {

    return jinj_token_list_append(tl, jinj_make_token_with_value(type, line, column,
                                                                 value, value_len));
}

JinjLexerResult jinj_token_list_extend(JinjTokenList* tl, JinjTokenList* other) {
    JinjLexerResult result = jinj_token_list_reserve(tl, tl->len + other->len);
    if (result.code != JinjLexerSuccess) return result;

    memcpy(tl->tokens + tl->len, other->tokens, other->len * sizeof(JinjToken));
    tl->len += other->len;
    return JINJ_LEXER_RESULT_SUCCESS;
}

JinjLexerResult jinj_token_list_reserve(JinjTokenList* tl, jinj_usize_t min_cap) {
    jinj_usize_t natural_grow = _JINJ_TOKEN_LIST_GROW(tl->cap);
    jinj_usize_t resize_to = min_cap > natural_grow ? min_cap : natural_grow;
    return _jinj_token_list_resize(tl, resize_to) == NULL ? ALLOC_FAILED_RESULT : JINJ_LEXER_RESULT_SUCCESS;
}

JinjLexerResult jinj_token_list_reserve_exact(JinjTokenList* tl, jinj_usize_t min_cap) {
    if (tl->cap < min_cap) {
        return _jinj_token_list_resize(tl, min_cap) == NULL ? ALLOC_FAILED_RESULT : JINJ_LEXER_RESULT_SUCCESS;
    }
    return JINJ_LEXER_RESULT_SUCCESS;
}
