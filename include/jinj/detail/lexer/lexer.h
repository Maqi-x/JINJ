#ifndef JINJ_DETAIL_LEXER_LEXER_H
#define JINJ_DETAIL_LEXER_LEXER_H

#include "token.h"
#include "tokenlist.h"

typedef struct JinjLexerResult {
    enum JinjLexerErrorCode {
        Ok,
        UnexpectedChar,
        UnterminatedStringLiteral,
        UnterminatedCharLiteral,
    } code;
    JinjTokenLocation location;
    union {
        char* owned;
        const char* view;
        void* other;
    } error_details;
} JinjLexerResult;

typedef struct JinjLexer {
    JinjTokenList tokens;
} JinjLexer;

#endif // JINJ_DETAIL_LEXER_LEXER_H
