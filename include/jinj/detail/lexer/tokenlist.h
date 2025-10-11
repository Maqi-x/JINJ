#ifndef JINJ_DETAIL_LEXER_TOKENLIST_H
#define JINJ_DETAIL_LEXER_TOKENLIST_H

#include "token.h"

typedef struct JinjTokenList {
    JinjToken* tokens;
    usize len;
    usize cap;
} JinjTokenList;

#endif // JINJ_DETAIL_LEXER_TOKENLIST_H
