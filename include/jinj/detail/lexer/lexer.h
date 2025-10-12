#ifndef JINJ_DETAIL_LEXER_LEXER_H
#define JINJ_DETAIL_LEXER_LEXER_H

#include "result.h"
#include "token.h"
#include "tokenlist.h"

typedef enum JinjLexerFlags {
    JinjLexerSaveComments = 1 << 0,
    JinjLexerAllowUnterminated = 1 << 1,
    JinjLexerTrimWhitespace = 1 << 2,
    JinjLexerAllowUtf8Idents = 1 << 3,
    JinjLexerSkipUnknown = 1 << 4,
} JinjLexerFlags;

#define JINJ_LEXER_FLAGS_DEFAULT \
    (JinjLexerSaveComments | JinjLexerTrimWhitespace)

typedef enum JinjLexerState {
    JinjLexerStateDefault = 0,

    JinjLexerStateParsingIdent,
    JinjLexerStateParsingNumber,
    JinjLexerStateParsingFloat,
    JinjLexerStateParsingChar,
    JinjLexerStateParsingString,

    JinjLexerStateDone,
} JinjLexerState;

typedef struct JinjLexer {
    JinjLexerFlags flags;

    const char* input;
    usize input_len;

    JinjTokenLocation location;
    usize pos;
    JinjTokenLocation token_start_location;
    usize token_start_pos;

    JinjLexerState state;
    JinjTokenList tokens;
} JinjLexer;

JinjLexerResult _jinj_lexer_add_token(JinjLexer* lexer, JinjTokenType type);
JinjLexerResult _jinj_lexer_add_token_with_value(JinjLexer* lexer, JinjTokenType type,
                                                 const char* value, usize value_len);

void jinj_lexer_init(JinjLexer* lexer, const char* input, usize input_len, JinjLexerFlags flags);
void jinj_lexer_deinit(JinjLexer* lexer);

JinjLexerResult jinj_lexer(JinjLexer* lexer);

#endif // JINJ_DETAIL_LEXER_LEXER_H
