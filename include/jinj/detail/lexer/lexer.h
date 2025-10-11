#ifndef JINJ_DETAIL_LEXER_LEXER_H
#define JINJ_DETAIL_LEXER_LEXER_H

#include "result.h"
#include "token.h"
#include "tokenlist.h"

typedef struct JinjLexer {
    const char* input;
    usize pos;

    JinjTokenList tokens;
} JinjLexer;

#endif // JINJ_DETAIL_LEXER_LEXER_H
