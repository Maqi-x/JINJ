#ifndef JINJ_DETAIL_LEXER_TOKENLIST_H
#define JINJ_DETAIL_LEXER_TOKENLIST_H

#include "result.h"
#include "token.h"

typedef struct JinjTokenList {
    JinjToken* tokens;
    usize len;
    usize cap;
} JinjTokenList;

JinjToken* _jinj_token_list_resize(JinjTokenList* tl, usize new_cap);

void jinj_token_list_init(JinjTokenList* tl);
void jinj_token_list_deinit(JinjTokenList* tl);

JinjLexerResult jinj_token_list_append(JinjTokenList* tl, JinjToken tok);
JinjLexerResult jinj_token_list_append_new(JinjTokenList* tl, JinjTokenType type, int line, int column);
JinjLexerResult jinj_token_list_append_new_with_value(JinjTokenList* tl, JinjTokenType type, int line, int column,
                                                      const char* value, usize value_len);

JinjLexerResult jinj_token_list_extend(JinjTokenList* tl, JinjTokenList* other);

JinjLexerResult jinj_token_list_reserve(JinjTokenList* tl, usize min_len);
JinjLexerResult jinj_token_list_reserve_exact(JinjTokenList* tl, usize min_len);


#endif // JINJ_DETAIL_LEXER_TOKENLIST_H
