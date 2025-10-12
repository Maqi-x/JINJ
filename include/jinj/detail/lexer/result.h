#ifndef JINJ_DETAIL_LEXER_RESULT_H
#define JINJ_DETAIL_LEXER_RESULT_H

#include "token.h"

typedef struct JinjLexerResult {
    enum JinjLexerErrorCode {
        JinjLexerSuccess = 0,

        JinjLexerErrorAllocFailed,
        JinjLexerErrorUnexpectedChar,

        JinjLexerErrorUnterminatedStringLiteral,
        JinjLexerErrorUnterminatedCharLiteral,

        JinjLexerErrorUnterminatedBlockComment,
    } code;
    JinjTokenLocation location;
    union {
        char c;
        JinjTokenLocation location;
        void* other;
    } error_details;
} JinjLexerResult;

#define JINJ_LEXER_RESULT_SUCCESS ((JinjLexerResult) { .code = JinjLexerSuccess, .error_details.other = NULL })

#endif // JINJ_DETAIL_LEXER_RESULT_H
